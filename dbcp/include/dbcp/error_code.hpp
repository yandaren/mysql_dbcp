/**
 *
 * dbcp error_code
 *
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-07
 */

#ifndef __ydk_dbcp_error_code_hpp__
#define __ydk_dbcp_error_code_hpp__

#include <cstdint>
#include <string>

namespace dbcp{

class error_code
{
protected:
    int32_t     value_;
    std::string msg_;

public:
    error_code() :value_(0){
    }

    error_code(int32_t value) : value_(value){
    }

    error_code(int32_t value, const char* msg) : value_(value), msg_(msg){
    }

    void    set_value(int32_t value){
        value_ = value;
    }

    int32_t value() const {
        return value_;
    }

    void    set_message(const char* msg){
        msg_ = msg;
    }

    const std::string& message() const{
        return msg_;
    }

    bool operator!() const
    {
        return value_ == 0;
    }

    explicit operator bool() const {
        return value_ != 0;
    }
};

}

#endif