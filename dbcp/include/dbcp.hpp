/**
 *
 * dbcp.hpp
 *
 * some global apis define
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_dbcp_hpp__
#define __ydk_dbcp_dbcp_hpp__

#include <dbcp/config.hpp>
#include <dbcp/connection_config.hpp>
#include <dbcp/connection.hpp>
#include <dbcp/connection_wrapper.hpp>
#include <dbcp/datasource.hpp>
#include <dbcp/driver.hpp>
#include <dbcp/prepared_statement.hpp>
#include <dbcp/transaction.hpp>
#include <dbcp/internal/logger_handler.hpp>
#include <dbcp/details/datasource_factory.hpp>

namespace dbcp{

   /**
    * @brief initialize the environment( mainy initialize the global logger handler)
    */
    static void initialize()
    {
        // initialize the static logger handler
        internal::logger_initialize();
    }

   /**
    * @brief set the custom log handler
    */
    static void set_log_handler(const internal::log_handler_func& func){
        internal::set_log_handler(func);
    }

   /**
    * @brief set the log level
    */
    static void set_log_lvl(internal::dbcp_log_level lvl){
        internal::set_log_lvl(lvl);
    }
}

#endif