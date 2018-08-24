/**
 *
 * driver
 *
 * driver interface
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_driver_hpp__
#define __ydk_dbcp_driver_hpp__

#include <dbcp/connection_config.hpp>

namespace dbcp{

class connection;
class driver
{
public:
    virtual ~driver() {}

public:
    /** 
     * @brief attempts to make a database connection to the gen url
     */
    virtual connection* connect(const connection_config& config) = 0;
};

}

#endif