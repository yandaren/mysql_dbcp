/**
 *
 * mysql_driver.hpp
 *
 * mysql connection driver
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_mysql_driver_hpp__
#define __ydk_dbcp_mysql_driver_hpp__

#include <dbcp/driver/mysql/mysql_config.hpp>
#include <dbcp/driver/mysql/mysql_connection.hpp>
#include <dbcp/driver.hpp>
#include <dbcp/connection_config.hpp>

namespace dbcp{
namespace mysql{

class mysql_driver : public driver
{
public:
    mysql_driver(){}
    virtual ~mysql_driver() {}

public:
    /**
    * @brief attempts to make a database connection to the gen url
    */
    connection* connect(const connection_config& config){
        mysql_connection* con = new mysql_connection();
        if (!con->initialize(config)){
            delete con;
            return nullptr;
        }
        return con;
    } 
};

/** get a driver instance */
static driver* create_driver_instance(){
    return new mysql_driver();
}

}
}

#endif