/**
 *
 * msyql connection
 *
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_mysql_connection_hpp__
#define __ydk_dbcp_mysql_connection_hpp__

#include <dbcp/driver/mysql/mysql_config.hpp>
#include <dbcp/driver/mysql/mysql_statement.hpp>
#include <dbcp/driver/mysql/mysql_resultset.hpp>
#include <dbcp/driver/mysql/mysql_prepared_statement.hpp>
#include <dbcp/internal/logger_handler.hpp>
#include <dbcp/connection.hpp>
#include <dbcp/basedefine.hpp>
#include <dbcp/datasource.hpp>
#include <thread>

namespace dbcp{
namespace mysql{

class mysql_connection : public connection
{
protected:
    bool                    is_closed_;
    bool                    is_auto_commit_;
    MYSQL*                  mysql_con_;
    connection_config       con_config_;

public:
    mysql_connection()
        : mysql_con_(NULL)
        , is_closed_(true)
        , is_auto_commit_(false)
    {
    }

    ~mysql_connection(){
        check_free_msyql_con();
    }

    int32_t get_ds_id(){
        if (ds_){
            return ds_->get_id();
        }
        return 0;
    }

    MYSQL* raw_con(){
        return mysql_con_;
    }

    /** 
     * @brief initialize 
     */
    bool    initialize(const connection_config& config){
        check_free_msyql_con();
        con_config_ = config;

        char* unix_socket = nullptr;
        ulong client_flag = 0;

        mysql_con_ = mysql_init(NULL);

        if (mysql_options(mysql_con_, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&config.connect_time_out)) {
            dbcp_log_error("try set connect time_out[%d]s option with config{%s} failed.",
                config.connect_time_out, config.to_string().c_str());

            return false;
        }

        if (!mysql_real_connect(
            mysql_con_,
            config.host.c_str(),
            config.user_name.c_str(),
            config.user_passwd.c_str(),
            config.db_name.c_str(),
            config.host_port,
            unix_socket,
            client_flag)){

            dbcp_log_error("connection to mysql server with config{%s} failed.",
                config.to_string().c_str());

            ::mysql_close(mysql_con_);
            mysql_con_ = nullptr;

            return false;
        }
        dbcp_log_info("connection dsid[%d] id[%d] to mysql server with config{%s] success.",
            get_ds_id(), get_id(), config.to_string().c_str());

        return true;
    }

    /** 
     * @brief try reconnect
     */
    bool try_reconnect(int32_t try_count = 1, int32_t interval_in_milli = 0){
        dbcp_log_info("pool[%d] id[%d] connection try reconect.",
            get_ds_id(), get_id());

        bool success = false;
        int32_t tryed_count = 0;
        do{
            success = initialize(con_config_);
            if (!success && interval_in_milli > 0){
                std::this_thread::sleep_for(std::chrono::milliseconds(interval_in_milli));
            }
            tryed_count++;
        } while (!success && tryed_count < try_count);

        return success;
    }

public:

    /** interface impls */
    virtual void    close() override{
        if (ds_){
            ds_->reclaim(this);
        }
    }

    virtual bool    commit() override{
        return !mysql_commit(mysql_con_);
    }

    virtual bool    setautocommit(bool auto_commit) override{
        bool ret = !mysql_autocommit(mysql_con_, (my_bool)auto_commit);
        if (ret) {
            is_auto_commit_ = auto_commit;
        }
        return ret;
    }

    virtual bool    isautocommit() override{
        return is_auto_commit_;
    }

    virtual bool    rollback() override{
        return !mysql_rollback(mysql_con_);
    }

    virtual prepared_statement_ptr prepared_statement(const char* sql) override{
        return prepared_statement_ptr(new mysql_prepared_statement(mysql_con_, sql));
    }

    /** some operate interface */

    /**
    * @brief get the result set of the sql
    */
    virtual resultset_ptr   query(const char* sql) override{
        mysql_statement stmt(mysql_con_);
        return resultset_ptr(stmt.execute_query(sql));
    }

    virtual resultset_ptr   query(const char* sql, error_code& err) override{
        mysql_statement stmt(mysql_con_);
        return resultset_ptr(stmt.execute_query(sql, err));
    }

    virtual resultset_ptr   query(prepared_statement_ptr stmt) override{
        return stmt->execute_query();
    }

    virtual resultset_ptr   query(prepared_statement_ptr stmt, error_code& err) override{
        return stmt->execute_query(err);
    }

    virtual int32_t         execute(const char* sql) override{
        mysql_statement stmt(mysql_con_);
        return stmt.execute_update(sql);
    }

    virtual int32_t         execute(const char* sql, error_code& err) override{
        mysql_statement stmt(mysql_con_);
        return stmt.execute_update(sql, err);
    }

    virtual int32_t         execute(prepared_statement_ptr stmt) override{
        return stmt->execute_update();
    }

    virtual int32_t         execute(prepared_statement_ptr stmt, error_code& err) override{
        return stmt->execute_update(err);
    }

protected:
    void    check_free_msyql_con(){
        if (mysql_con_){
            ::mysql_close(mysql_con_);
            mysql_con_ = nullptr;
        }
    }
};
}
}

#endif