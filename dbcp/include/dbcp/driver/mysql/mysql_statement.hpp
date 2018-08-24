/**
 *
 * mysql_statement.hpp
 *
 * mysql_statement impl
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_mysql_statement_hpp__
#define __ydk_dbcp_mysql_statement_hpp__

#include <string.h>
#include <dbcp/driver/mysql/mysql_config.hpp>
#include <dbcp/driver/mysql/mysql_resultset.hpp>
#include <dbcp/statement.hpp>
#include <memory>

namespace dbcp{
namespace mysql{

class mysql_statement : public statement
{
public:
    typedef std::shared_ptr<mysql_statement> ptr;
protected:
    MYSQL*        mysql_con_;
public:
    mysql_statement(MYSQL* mysql_con)
        : mysql_con_(mysql_con){
    }

    virtual ~mysql_statement() {
    }

public:

    virtual resultset_ptr   execute_query(const char* sql) override{
        error_code err;
        return execute_query(sql, err);
    }

    virtual resultset_ptr   execute_query(const char* sql, error_code& err) override{
        // initialize error_code
        err.set_value(0);

        if (!mysql_con_){
            return resultset_ptr();
        }

        uint32_t sql_len = (uint32_t)strlen(sql);
        int32_t ret = ::mysql_real_query(mysql_con_, sql, sql_len);
        if (ret){
            err.set_value(ret);
            err.set_message(::mysql_error(mysql_con_));

            return resultset_ptr();
        }

        MYSQL_RES* res = ::mysql_store_result(mysql_con_);
        if (res){
            return resultset_ptr(new mysql_resultset(res));
        }
        else{
            err.set_value(::mysql_errno(mysql_con_));
            err.set_message(::mysql_error(mysql_con_));
        }

        return resultset_ptr();
    }

    virtual int32_t         execute_update(const char* sql) override{
        error_code err;
        return execute_update(sql, err);
    }

    virtual int32_t         execute_update(const char* sql, error_code& err) override{
        // initialize error_code
        err.set_value(0);

        if (!mysql_con_){
            return -1;
        }

        uint32_t sql_len = (uint32_t)strlen(sql);
        int32_t ret = ::mysql_real_query(mysql_con_, sql, sql_len);
        if (ret){
            err.set_value(ret);
            err.set_message(::mysql_error(mysql_con_));

            return -1;
        }

        int32_t affect_rows = (int32_t)::mysql_affected_rows(mysql_con_);
        if (affect_rows == -1){
            err.set_value(::mysql_errno(mysql_con_));
            err.set_message(::mysql_error(mysql_con_));
        }

        return affect_rows;
    }

    virtual int32_t         fetch_size() override{
        return 0;
    }

    virtual void            set_fetch_size(int32_t size) override{
    }

    virtual bool            more_result() override{
        if (!mysql_con_){
            return false;
        }

        if (!!::mysql_more_results(mysql_con_)){
            int32_t ret = ::mysql_next_result(mysql_con_);
            if (ret == 0){
                return ::mysql_field_count(mysql_con_) > 0;
            }
        }

        return false;
    }

};

}
}

#endif
