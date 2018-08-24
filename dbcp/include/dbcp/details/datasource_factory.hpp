/**
 *
 * datasource_factory.hpp
 *
 * datasource create factory
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-25
 */

#ifndef __ydk_dbcp_details_data_source_factory_hpp__
#define __ydk_dbcp_details_data_source_factory_hpp__

#include <dbcp/config.hpp>
#include <dbcp/details/datasource_impl.hpp>
#include <dbcp/driver/mysql/mysql_driver.hpp>
#include <dbcp/internal/logger_handler.hpp>
#include <unordered_map>

namespace dbcp{
namespace detail{

class datasource_factory
{
protected:
    /** <db type, driver* > */
    std::unordered_map<std::string, driver*> drivers_map_;

public:
    datasource_factory(){
    }

    virtual ~datasource_factory(){
        for (auto& kv : drivers_map_){
            delete kv.second;
        }
        drivers_map_.clear();
    }

    /** create a instance */
    static datasource_factory* create_instance(){
        return new datasource_factory();
    }

public:
    /** 
     * @brief get datasource driver
     */
    driver* get_driver(const std::string& db_type){
        auto iter = drivers_map_.find(db_type);
        if (iter != drivers_map_.end()){
            return iter->second;
        }
        else{
            driver* dv = create_driver(db_type);
            if (dv){
                drivers_map_[db_type] = dv;
                return dv;
            }
        }
        return nullptr;
    }

    /** 
     * @brief create datasource
     * @param db_type - "mysql", "sqlserver", "oracle" and so on( but only support mysql yet)
     */
    datasource* create(const std::string& db_type, const connection_config& config, int32_t init_pool_size, int32_t max_pool_size){
        driver* dv = get_driver(db_type);
        if (!dv){
            return nullptr;
        }

        return new datasource_impl(dv, config, init_pool_size, max_pool_size);
    }

protected:
    driver* create_driver(const std::string& db_type){
        // only support msyql driver 
        if (db_type == "mysql"){
            return mysql::create_driver_instance();
        }
        else{
            dbcp_log_fatal("unsupport db_type[%s], only support mysql driver yet.",
                db_type.c_str());

            return nullptr;
        }
    }

};

}
}

#endif