/**
 *
 * connection connect config
 *
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_connection_config_hpp__
#define __ydk_dbcp_connection_config_hpp__

#include <cstdint>
#include <string>
#include <sstream>

namespace dbcp{

struct connection_config
{
    std::string host;           // host ip( or hostname)
    int32_t     host_port;      // server port
    std::string user_name;      // user name
    std::string user_passwd;    // user password
    std::string db_name;        // database name
    int32_t     connect_time_out;   // connect time out in seconds

    connection_config() : host("127.0.0.1"), host_port(3306), connect_time_out(5) {
    }

    std::string to_string() const {
        std::stringstream ss;
        ss << "mysql://" << user_name << ":" << user_passwd << "@" << host << ":" << host_port << "/" << db_name;
        return std::move(ss.str());
    }
};
}

#endif