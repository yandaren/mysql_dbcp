/**
 *
 * mysql_param_databind.hpp
 *
 * mysql param data bind
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-11
 */

#ifndef __ydk_dbcp_mysql_param_databind_hpp__
#define __ydk_dbcp_mysql_param_databind_hpp__

#include <dbcp/driver/mysql/mysql_config.hpp>
#include <dbcp/driver/mysql/mysql_bind_buffer.hpp>
#include <dbcp/internal/logger_handler.hpp>
#include <vector>

namespace dbcp{
namespace mysql{

class mysql_param_databind
{
protected:
    struct bind_helper_data_t{
        unsigned long len;
        bind_buffer* buf;
        uint8_t      is_unsigned;
        uint8_t      is_null;

        bind_helper_data_t() :buf(NULL), len(0), is_unsigned(0), is_null(0){
        }

        ~bind_helper_data_t(){
            release_buf();
        }

        void release_buf(){
            if (buf){
                delete buf;
                buf = nullptr;
            }
        }
    };

public:
    MYSQL_BIND*     param_bind_;
    std::vector<bind_helper_data_t*>        bind_helper_data_;
    std::unordered_map<int32_t, int32_t>    param_index_vector_index_map_;
    MYSQL_STMT*     mysql_stmt_;

public:
    mysql_param_databind(MYSQL_STMT* stmt)
        : mysql_stmt_(stmt)
        , param_bind_(nullptr)
    {
    }

    ~mysql_param_databind(){

        release();

        // clear bind helper data
        for (auto& bind_data : bind_helper_data_){
            if (bind_data){
                delete bind_data;
                bind_data = nullptr;
            }
        }
        bind_helper_data_.clear();
    }

public:
    void set_bigint(int32_t param_index, const std::string& value) {
        set_string(param_index, value);
    }

    void set_blob(int32_t param_index, const std::string& value){
        set_string(param_index, value);
    }

    void set_boolean(int32_t param_index, bool value){
        set_int32(param_index, (int32_t)value);
    }

    void set_datatime(int32_t param_index, const std::string& value){
        set_string(param_index, value);
    }

    void set_double(int32_t param_index, double value) {
        if (param_index < 0)
            return;

        // check has set
        enum_field_types type = MYSQL_TYPE_DOUBLE;
        bind_helper_data_t* bind_data = try_get_or_allocate_data(param_index);
        bind_data->buf = new bind_buffer(8, type);
        *((double*)bind_data->buf->buffer) = value;
    }

    void set_int32(int32_t param_index, int32_t value) {
        enum_field_types type = MYSQL_TYPE_LONG;
        bind_helper_data_t* bind_data = try_get_or_allocate_data(param_index);
        bind_data->buf = new bind_buffer(4, type);
        *((int32_t*)bind_data->buf->buffer) = value;
    }

    void set_uint32(int32_t param_index, uint32_t value) {
        enum_field_types type = MYSQL_TYPE_LONG;
        bind_helper_data_t* bind_data = try_get_or_allocate_data(param_index);
        bind_data->buf = new bind_buffer(4, type);
        bind_data->is_unsigned = 1;
        *((uint32_t*)bind_data->buf->buffer) = value;
    }

    void set_int64(int32_t param_index, int64_t value) {
        enum_field_types type = MYSQL_TYPE_LONGLONG;
        bind_helper_data_t* bind_data = try_get_or_allocate_data(param_index);
        bind_data->buf = new bind_buffer(8, type);
        *((int64_t*)bind_data->buf->buffer) = value;
    }

    void set_uint64(int32_t param_index, uint64_t value){
        enum_field_types type = MYSQL_TYPE_LONGLONG;
        bind_helper_data_t* bind_data = try_get_or_allocate_data(param_index);
        bind_data->buf = new bind_buffer(8, type);
        bind_data->is_unsigned = 1;
        *((uint64_t*)bind_data->buf->buffer) = value;
    }

    void set_string(int32_t param_index, const std::string& value){
        enum_field_types type = MYSQL_TYPE_STRING;
        bind_helper_data_t* bind_data = try_get_or_allocate_data(param_index);
        bind_data->buf = new bind_buffer(value.length() + 1, type);
        memcpy(bind_data->buf->buffer, value.c_str(), value.length());
        bind_data->buf->buffer[value.length()] = '\0';
        bind_data->len = (unsigned long)value.length();
    }

    void set_null(int32_t param_index) {
        enum_field_types type = MYSQL_TYPE_NULL;
        bind_helper_data_t* bind_data = try_get_or_allocate_data(param_index);
        bind_data->buf = new bind_buffer(0, type);
        bind_data->is_null = 1;
    }

    bool bind(error_code* error){
        // initialize error code
        if (error){
            error->set_value(0);
        }

        release();
        int32_t param_count = ::mysql_stmt_param_count(mysql_stmt_);
        if (param_count == 0){
            return true;
        }

        param_bind_ = new MYSQL_BIND[param_count];
        memset(param_bind_, 0, sizeof(MYSQL_BIND) * param_count);

        for (int32_t i = 0; i < param_count; ++i){
            bind_helper_data_t* bind_data = get_data(i);
            if (!bind_data){
                dbcp_log_error("param bind error, param index[%d] not bind yet.", i);
                return false;
            }

            MYSQL_BIND& param = param_bind_[i];
            param.buffer_type = bind_data->buf->type;
            param.buffer = bind_data->buf->buffer;
            param.buffer_length = (uint32_t)(bind_data->buf->type == MYSQL_TYPE_STRING ? bind_data->buf->size : 0);
            param.length = bind_data->buf->type == MYSQL_TYPE_STRING ? &bind_data->len : 0;
            param.is_null_value = bind_data->is_null;
        }

        int32_t ret = ::mysql_stmt_bind_param(mysql_stmt_, param_bind_);
        if (ret){
            if (error){
                error->set_value(ret);
                error->set_message(::mysql_stmt_error(mysql_stmt_));
            }
        }
        return ret == 0;
    }

protected:
    void release(){
        if (param_bind_){
            delete[] param_bind_;
            param_bind_ = nullptr;
        }
    }

    bind_helper_data_t* try_get_or_allocate_data(int32_t param_index){
        bind_helper_data_t* data = get_data(param_index);
        if ( !data ){
            data = new bind_helper_data_t();
            bind_helper_data_.push_back(data);
            param_index_vector_index_map_[param_index] = (int32_t)(bind_helper_data_.size() - 1);
        }
        data->release_buf();
        return data;
    }

    int32_t get_vector_index(int32_t param_index){
        auto iter = param_index_vector_index_map_.find(param_index);
        if (iter != param_index_vector_index_map_.end()){
            return iter->second;
        }
        return -1;
    }

    bind_helper_data_t* get_data(int32_t param_index){
        int32_t vector_index = get_vector_index(param_index);
        if (vector_index >= 0 && vector_index < (int32_t)bind_helper_data_.size()){
            return bind_helper_data_[vector_index];
        }
        return nullptr;
    }
};
}
}

#endif