/**
 *
 * datasource_impl.hpp
 *
 * the connection pool impl
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-12
 */

#ifndef __ydk_dbcp_detail_data_source_impl_hpp__
#define __ydk_dbcp_detail_data_source_impl_hpp__

#include <dbcp/config.hpp>
#include <dbcp/datasource.hpp>
#include <dbcp/connection.hpp>
#include <dbcp/driver.hpp>
#include <dbcp/connection_config.hpp>
#include <dbcp/resultset.hpp>
#include <dbcp/internal/logger_handler.hpp>
#include <utility/asio_base/thread_pool.hpp>
#include <utility/asio_base/timer.hpp>
#include <mutex>
#include <queue>
#include <list>
#include <algorithm>

namespace dbcp{
namespace detail{

/** ping interval in milliseconds */
static const int32_t ping_interval = 5000;

class datasource_impl : public datasource
{
protected:
    int32_t                     init_pool_size_;
    int32_t                     max_pool_size_;
    driver*                     driver_;
    connection_config           config_;
    std::queue<connection*>     free_con_list_;
    std::list<connection*>    total_con_list_;
    int32_t                     next_con_id_;
    std::mutex                  mtx_;
    utility::asio_base::thread_pool* thread_pool_;
    utility::asio_base::timer::ptr   ping_timer_;

public:
    datasource_impl(driver* dv, const connection_config& config, int32_t init_pool_size, int32_t max_pool_size)
        : init_pool_size_(init_pool_size)
        , max_pool_size_(max_pool_size)
        , driver_(dv)
        , config_(config)
        , next_con_id_(0)
    {
        if (init_pool_size_ > max_pool_size_){
            init_pool_size_ = max_pool_size_;
        }

        for (int32_t i = 0; i < max_pool_size_; ++i){
            try_create_new_connection();
        }

        dbcp_log_info("ds[%p] init finish, init/max[%d%d], free/total[%zd/%zd].",
            this, init_pool_size_, max_pool_size_, free_con_list_.size(), total_con_list_.size());

        thread_pool_ = new utility::asio_base::thread_pool(1);
        thread_pool_->start();

        ping_timer_ = utility::asio_base::timer::create(thread_pool_->io_service());
        ping_timer_->register_handler(std::bind(&datasource_impl::ping_timer_handler, 
            this, std::placeholders::_1, std::placeholders::_2));
        ping_timer_->start(ping_interval);
    }

    ~datasource_impl(){
        if (ping_timer_){
            ping_timer_->cancel();
        }

        if (thread_pool_){
            thread_pool_->stop();
            thread_pool_->wait_for_stop();
        }
    }

public:

    /** implements of interfaces of datasource */

    /**
    * @brief try get a connection from the datasource
    */
    virtual connection* get_connection() override{

        std::lock_guard<std::mutex> locker(mtx_);

        if (free_con_list_.empty()){
            try_create_new_connection();
        }

        if (free_con_list_.empty()){
            return nullptr;
        }

        connection* con = free_con_list_.front();
        free_con_list_.pop();

        return con;
    }

    /**
    * @brief try return the connection to the pool
    */
    virtual void        reclaim(connection* con) override{
        std::lock_guard<std::mutex> locker(mtx_);
        free_con_list_.push(con);
    }


protected:

    connection* create_new_connection(){
        connection* con = driver_->connect(config_);
        if (con){
            con->set_ds(this);
        }
        return con;
    }

    void try_create_new_connection(){
        if ((int32_t)total_con_list_.size() >= max_pool_size_){
            dbcp_log_error("ds[%p] id[%d] cur_total_con[%zd] reach max[%d], free[%zd], try create new con failed.",
                this, get_id(), total_con_list_.size(), max_pool_size_, free_con_list_.size());
            return;
        }

        connection* con = create_new_connection();
        if (con && !add_connection_nolock(con)){
            delete con;
        }
    }

    bool add_connection_nolock(connection* con){
        if (!con)
            return false;

        if ((int32_t)total_con_list_.size() >= max_pool_size_){
            return false;
        }

        total_con_list_.push_back(con);
        free_con_list_.push(con);

        dbcp_log_info("add a new connection[%p] id[%d] to ds[%p] id[%d], cur free/total[%zd/z%d], max[%d].",
            con, con->get_id(), this, get_id(), free_con_list_.size(), total_con_list_.size(), max_pool_size_);

        return true;
    }

    bool add_connection(connection* con){
        std::lock_guard<std::mutex> locker(mtx_);

        return add_connection_nolock(con);
    }

    int32_t free_con_count(){
        std::lock_guard<std::mutex> locker(mtx_);
        return (int32_t)free_con_list_.size();
    }

    void free_a_connection(connection* con){
        if (!con){
            return;
        }

        // remove from the pool
        {
            std::lock_guard<std::mutex> locker(mtx_);

            auto iter = std::find(total_con_list_.begin(), total_con_list_.end(), con);
            if (iter != total_con_list_.end()){
                total_con_list_.erase(iter);
            }

            dbcp_log_info("ds[%p] id[%d] try free a con[%p] id[%d], cur free/total[%zd/%zd], max[%d].",
                this, get_id(), con, con->get_id(), free_con_list_.size(), total_con_list_.size(), max_pool_size_);
        }

        delete con;
    }

    void ensure_min_connection_count(){
        int32_t total_count = 0;
        {
            std::lock_guard<std::mutex> locker(mtx_);
            total_count = (int32_t)total_con_list_.size();
        }

        if (total_count < init_pool_size_){
            for (int32_t i = 0; i < init_pool_size_ - total_count; ++i){
                connection* con = create_new_connection();
                if (con && !add_connection(con)){
                    delete con;
                }
            }
        }
    }

    void ping(){
        int32_t need_check_count = free_con_count();
        for (int32_t i = 0; i < need_check_count; ++i){
            connection* con = get_connection();
            if (con){
                resultset_ptr result = con->query(get_validation_query_sql().c_str());
                if (!result.get()){
                    dbcp_log_error("ds[%p] id[%d] con[%p] id[%d] ping failed, try free the con.",
                        this, get_id(), con, con->get_id());

                    free_a_connection(con);
                }
                else{
                    con->close();
                }
            }
        }

        ensure_min_connection_count();
    }

    void ping_timer_handler(utility::asio_base::timer::ptr timer, const asio::error_code& error){
        if (!error){
            ping();

            ping_timer_->start(ping_interval);
        }
        else{
            dbcp_log_error("ds ping_timer_handler, error[%s:%d].", error.message().c_str(), error.value());
        }
    }
};

}
}

#endif
