/**
 *
 * prepared_statement.hpp
 *
 * prepared statement interface
 * 
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_prepared_statement_hpp__
#define __ydk_dbcp_prepared_statement_hpp__

#include <cstdint>
#include <dbcp/statement.hpp>

namespace dbcp{

class prepared_statement : public statement
{
public:
    virtual ~prepared_statement() {}

public:

    /** param interface */
    virtual void set_bigint(int32_t param_index, const std::string& value) = 0;
    virtual void set_blob(int32_t param_index, const std::string& value) = 0;
    virtual void set_boolean(int32_t param_index, bool value) = 0;
    virtual void set_datatime(int32_t param_index, const std::string& value) = 0;
    virtual void set_double(int32_t param_index, double value) = 0;
    virtual void set_int32(int32_t param_index, int32_t value) = 0;
    virtual void set_uint32(int32_t param_index, uint32_t value) = 0;
    virtual void set_int64(int32_t param_index, int64_t value) = 0;
    virtual void set_uint64(int32_t param_index, uint64_t value) = 0;
    virtual void set_string(int32_t param_index, const std::string& value) = 0;
    virtual void set_null(int32_t param_index) = 0;

    /** operator interface */
    virtual resultset_ptr   execute_query() = 0;
    virtual resultset_ptr   execute_query(error_code& err) = 0;
    virtual int32_t         execute_update() = 0;
    virtual int32_t         execute_update(error_code& err) = 0;
};
}

#endif