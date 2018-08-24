/**
 *
 * connection pointer wrapper
 *
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_connection_wrapper_hpp__
#define __ydk_dbcp_connection_wrapper_hpp__

#include <dbcp/connection.hpp>

namespace dbcp{

class connection_wrapper
{
protected:
    connection* const raw_con_;

public:
    connection_wrapper(connection* con)
        : raw_con_(con){
    }

    ~connection_wrapper(){
        if (raw_con_){
            raw_con_->close();
        }
    }

    connection* operator->() const{
        return raw_con_;
    }

    connection* get() const{
        return raw_con_;
    }

    explicit operator bool() const {
        return get() != nullptr;
    }
};
}

#endif