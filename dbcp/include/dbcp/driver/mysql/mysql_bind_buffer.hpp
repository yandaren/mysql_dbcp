/**
 *
 * msyql bind buffer
 *
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-11
 */

#ifndef __ydk_dbcp_mysql_bind_buffer_hpp__
#define __ydk_dbcp_mysql_bind_buffer_hpp__

#include <dbcp/driver/mysql/mysql_config.hpp>

namespace dbcp{
namespace mysql{

struct bind_buffer{
    char*               buffer;
    size_t              size;
    enum_field_types    type;

    bind_buffer(size_t s, enum_field_types t)
        : buffer(nullptr)
        , size(0)
        , type(MYSQL_TYPE_NULL)
    {
        initialize(s, t);
    }

    bind_buffer(const MYSQL_FIELD* field)
        : buffer(nullptr)
        , size(0)
        , type(MYSQL_TYPE_NULL)
    {
        initialize(field);
    }

    ~bind_buffer(){
        if (buffer){
            delete[] buffer;
            buffer = nullptr;
        }
        size = 0;
    }

    void initialize(size_t s, enum_field_types t){
        if (buffer){
            delete[] buffer;
            buffer = nullptr;
        }

        size = s;
        type = t;

        if (size > 0){
            buffer = new char[size];
        }
        else{
            buffer = nullptr;
        }
    }

    void initialize(const MYSQL_FIELD* field){
        enum_field_types t = field->type;
        size_t s = get_field_type_size(field);
        if (t == MYSQL_TYPE_YEAR){
            t = MYSQL_TYPE_SHORT;
            s = 2;
        }

        initialize(s, t);
    }

    static size_t get_field_type_size(const MYSQL_FIELD* field){
        switch (field->type){
        case MYSQL_TYPE_NULL:
            return 0;
        case MYSQL_TYPE_TINY:
            return 1;
        case MYSQL_TYPE_SHORT:
            return 2;
        case MYSQL_TYPE_INT24:
        case MYSQL_TYPE_LONG:
        case MYSQL_TYPE_FLOAT:
            return 4;
        case MYSQL_TYPE_DOUBLE:
        case MYSQL_TYPE_LONGLONG:
            return 8;
        case MYSQL_TYPE_YEAR:
            return 2;
        case MYSQL_TYPE_TIMESTAMP:
        case MYSQL_TYPE_DATE:
        case MYSQL_TYPE_TIME:
        case MYSQL_TYPE_DATETIME:
            return sizeof(MYSQL_TIME);
        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_BLOB:
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_JSON:
            return field->max_length + 1;
        case MYSQL_TYPE_DECIMAL:
        case MYSQL_TYPE_NEWDECIMAL:
            return 64;
        case MYSQL_TYPE_BIT:
            return 8;
        default:
            return 0;
        }
    }
};
}
}

#endif