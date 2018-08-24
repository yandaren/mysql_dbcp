/**
 *
 * mysql_prepared_statement.hpp
 * 
 * mysql_prepared_statement implement
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_mysql_prepared_statement_hpp__
#define __ydk_dbcp_mysql_prepared_statement_hpp__

#include <dbcp/driver/mysql/mysql_config.hpp>
#include <dbcp/driver/mysql/mysql_prepared_resultset.hpp>
#include <dbcp/driver/mysql/mysql_param_databind.hpp>
#include <dbcp/prepared_statement.hpp>

namespace dbcp{
namespace mysql{

class mysql_prepared_statement : public prepared_statement
{
protected:
    MYSQL*        mysql_con_;
    MYSQL_STMT*   mysql_stmt_;
    std::string   sql_;
    mysql_param_databind* param_bind_;

public:
    mysql_prepared_statement(MYSQL* mysql_con, const char* sql) 
        : mysql_con_(mysql_con)
        , sql_(sql)
        , param_bind_(nullptr)
    {
        mysql_stmt_ = ::mysql_stmt_init(mysql_con_);
        param_bind_ = new mysql_param_databind(mysql_stmt_);
    }

    ~mysql_prepared_statement() {
        if (mysql_stmt_){
            ::mysql_stmt_close(mysql_stmt_);
            mysql_stmt_ = nullptr;
        }
        if (param_bind_){
            delete param_bind_;
            param_bind_ = nullptr;
        }
    }

public:

    /** implement of interfaces from prepared_statement */

    virtual void set_bigint(int32_t param_index, const std::string& value) override{
        if (param_bind_){
            param_bind_->set_bigint(param_index, value);
        }
    }

    virtual void set_blob(int32_t param_index, const std::string& value) override{
        if (param_bind_){
            param_bind_->set_blob(param_index, value);
        }
    }

    virtual void set_boolean(int32_t param_index, bool value) override{
        if (param_bind_){
            param_bind_->set_boolean(param_index, value);
        }
    }

    virtual void set_datatime(int32_t param_index, const std::string& value) override{
        if (param_bind_){
            param_bind_->set_datatime(param_index, value);
        }
    }

    virtual void set_double(int32_t param_index, double value) override{
        if (param_bind_){
            param_bind_->set_double(param_index, value);
        }
    }

    virtual void set_int32(int32_t param_index, int32_t value) override{
        if (param_bind_){
            param_bind_->set_int32(param_index, value);
        }
    }

    virtual void set_uint32(int32_t param_index, uint32_t value) override{
        if (param_bind_){
            param_bind_->set_uint32(param_index, value);
        }
    }

    virtual void set_int64(int32_t param_index, int64_t value) override{
        if (param_bind_){
            param_bind_->set_int64(param_index, value);
        }
    }

    virtual void set_uint64(int32_t param_index, uint64_t value) override{
        if (param_bind_){
            param_bind_->set_uint64(param_index, value);
        }
    }

    virtual void set_string(int32_t param_index, const std::string& value) override{
        if (param_bind_){
            param_bind_->set_string(param_index, value);
        }
    }

    virtual void set_null(int32_t param_index) override{
        if (param_bind_){
            param_bind_->set_null(param_index);
        }
    }

    virtual resultset_ptr   execute_query(const char* sql) override{
        return resultset_ptr();
    }

    virtual resultset_ptr   execute_query(const char* sql, error_code& err) override{
        return resultset_ptr();
    }

    virtual int32_t         execute_update(const char* sql) override{
        return 0;
    }

    virtual int32_t         execute_update(const char* sql, error_code& err) override{
        return 0;
    }

    virtual resultset_ptr   execute_query() override{
        error_code err;
        return execute_query(err);
    }

    virtual resultset_ptr   execute_query(error_code& err) override{
        // initialize the error code
        err.set_value(0);

        if (!mysql_stmt_){
            return resultset_ptr();
        }

        int32_t ret = ::mysql_stmt_prepare(mysql_stmt_, sql_.c_str(), (uint32_t)sql_.size());
        if (ret != 0){
            err.set_value(ret);
            err.set_message(::mysql_stmt_error(mysql_stmt_));

            return resultset_ptr();
        }

        if (!param_bind_->bind(&err)){
            return resultset_ptr();
        }

        ret = ::mysql_stmt_execute(mysql_stmt_);
        if (ret != 0){
            err.set_value(ret);
            err.set_message(::mysql_stmt_error(mysql_stmt_));

            return resultset_ptr();
        }

        my_bool attr_value = 1;
        ret = ::mysql_stmt_attr_set(mysql_stmt_, STMT_ATTR_UPDATE_MAX_LENGTH, &attr_value);
        if (ret != 0){
            err.set_value(ret);
            err.set_message(::mysql_stmt_error(mysql_stmt_));

            return resultset_ptr();
        }

        // store result
        ret = ::mysql_stmt_store_result(mysql_stmt_);
        if (ret != 0){
            err.set_value(ret);
            err.set_message(::mysql_stmt_error(mysql_stmt_));

            return resultset_ptr();
        }

        return resultset_ptr(new mysql_prepared_resultset(mysql_stmt_));
    }

    virtual int32_t         execute_update() override{
        error_code err;
        return execute_update(err);
    }

    virtual int32_t         execute_update(error_code& err) override{
        // initialize the error code
        err.set_value(0);

        if (!mysql_stmt_){
            return -1;
        }

        int32_t ret = ::mysql_stmt_prepare(mysql_stmt_, sql_.c_str(), (uint32_t)sql_.size());
        if (ret != 0){
            err.set_value(ret);
            err.set_message(::mysql_stmt_error(mysql_stmt_));

            return -1;
        }

        if (!param_bind_->bind(&err)){
            return -1;
        }

        ret = ::mysql_stmt_execute(mysql_stmt_);
        if (ret != 0){
            err.set_value(ret);
            err.set_message(::mysql_stmt_error(mysql_stmt_));

            return -1;
        }

        int32_t effect_rows = (int32_t)::mysql_stmt_affected_rows(mysql_stmt_);
        if (effect_rows == -1){
            err.set_value(::mysql_stmt_errno(mysql_stmt_));
            err.set_message(::mysql_stmt_error(mysql_stmt_));
        }
        return effect_rows;
    }

    virtual int32_t         fetch_size() override{
        return 0;
    }

    virtual void            set_fetch_size(int32_t size) override{

    }

    virtual bool            more_result() override{
        if (!mysql_stmt_){
            return false;
        }

        if (!mysql_con_){
            return false;
        }

        if (!!::mysql_more_results(mysql_con_)){
            int32_t ret = ::mysql_stmt_next_result(mysql_stmt_);
            if (ret == 0){
                return ::mysql_stmt_field_count(mysql_stmt_) > 0;
            }
        }

        return false;
    }
};
}
}

#endif