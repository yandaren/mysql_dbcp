/**
 *
 * mysql_result_databind.hpp
 *
 * mysql result data bind
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-10
 */

#ifndef __ydk_dbcp_mysql_result_databind_hpp__
#define __ydk_dbcp_mysql_result_databind_hpp__

#include <string.h>
#include <dbcp/driver/mysql/mysql_config.hpp>
#include <dbcp/driver/mysql/mysql_resultset_metadata.hpp>
#include <dbcp/driver/mysql/mysql_bind_buffer.hpp>

namespace dbcp{
namespace mysql{

class mysql_result_databind
{
protected:
    struct bind_helper_data_t{
        char         is_null;
        char         err;
        unsigned long len;
        bind_buffer* buf;

        bind_helper_data_t() :buf(NULL), err(0), len(0){
        }

        ~bind_helper_data_t(){
            if (buf){
                delete buf;
                buf = nullptr;
            }
        }
    };

public:
    int32_t             num_fields_;
    MYSQL_BIND*         result_bind_;
    bind_helper_data_t* bind_helper_data_;
    mysql_resultset_metadata* meta_data_;
    MYSQL_STMT*         mysql_stmt_;

public:
    mysql_result_databind(MYSQL_STMT* stmt, mysql_resultset_metadata* meta_data)
        : mysql_stmt_(stmt)
        , meta_data_(meta_data)
        , num_fields_(0)
        , result_bind_(nullptr)
        , bind_helper_data_(nullptr)
    {
    }

    ~mysql_result_databind(){
        release();
    }

public:
    int32_t bind_result(){
        release();

        if (!mysql_stmt_ || !meta_data_){
            return false;
        }

        num_fields_ = meta_data_->get_column_count();
        if (!num_fields_)
            return false;

        bind_helper_data_ = new bind_helper_data_t[num_fields_];
        memset(bind_helper_data_, 0, sizeof(bind_helper_data_t) * num_fields_);
        result_bind_ = new MYSQL_BIND[num_fields_];
        memset(result_bind_, 0, sizeof(MYSQL_BIND) * num_fields_);

        for (int32_t i = 0; i < num_fields_; ++i){
            MYSQL_FIELD* field = meta_data_->get_field(i);
            bind_helper_data_t& bind_helper_data = bind_helper_data_[i];
            bind_helper_data.buf = new bind_buffer(field);
            bind_buffer* bind_buf = bind_helper_data.buf;
            MYSQL_BIND& rbind = result_bind_[i];
            rbind.buffer_type = bind_buf->type;
            rbind.buffer = bind_buf->buffer;
            rbind.buffer_length = static_cast<unsigned long>(bind_buf->size);
            rbind.length = &bind_helper_data.len;
            rbind.is_null = &bind_helper_data.is_null;
            rbind.error = &bind_helper_data.err;
            rbind.is_unsigned = field->flags & UNSIGNED_FLAG;
        }

        return (int32_t)::mysql_stmt_bind_result(mysql_stmt_, result_bind_);
    }

protected:
    void release(){
        if (result_bind_){
            delete[] result_bind_;
            result_bind_ = nullptr;
        }

        if (bind_helper_data_){
            delete[] bind_helper_data_;
            bind_helper_data_ = nullptr;
        }
    }
};

}
}

#endif
