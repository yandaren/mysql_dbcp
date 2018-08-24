/**
 *
 * statement.hpp
 *
 * statement interface
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_statement_hpp__
#define __ydk_dbcp_statement_hpp__

#include <dbcp/basedefine.hpp>
#include <dbcp/error_code.hpp>

namespace dbcp{

class connection;
class statement
{
public:
    virtual ~statement() {}

protected:
    virtual resultset_ptr   execute_query(const char* sql) = 0;
    virtual resultset_ptr   execute_query(const char* sql, error_code& err) = 0;
    virtual int32_t         execute_update(const char* sql) = 0;
    virtual int32_t         execute_update(const char* sql, error_code& err) = 0;
    virtual int32_t         fetch_size() = 0;
    virtual void            set_fetch_size(int32_t size) = 0;
    virtual bool            more_result() = 0;
};
}

#endif