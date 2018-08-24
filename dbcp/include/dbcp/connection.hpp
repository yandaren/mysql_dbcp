/**
 *
 * connection
 *
 * connection interface
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_connection_hpp__
#define __ydk_dbcp_connection_hpp__

#include <cstdint>
#include <dbcp/basedefine.hpp>
#include <dbcp/error_code.hpp>

namespace dbcp{
class dirver;
class datasource;
class connection
{
protected:
    int32_t id_;
    datasource* ds_;

public:
    connection() : id_(0), ds_(0){}
    virtual ~connection(){}

    /** set/get id*/
    void    set_id(int32_t id){
        id_ = id;
    }

    int32_t get_id() const {
        return id_;
    }

    /** set/get datasource */
    void    set_ds(datasource* ds){
        ds_ = ds;
    }

    datasource* get_ds(){
        return ds_;
    }

public:
    /** connection interface */
    virtual void    close() = 0;
    virtual bool    commit() = 0;
    virtual bool    setautocommit(bool auto_commit) = 0;
    virtual bool    isautocommit() = 0;
    virtual bool    rollback() = 0;
    virtual prepared_statement_ptr prepared_statement(const char* sql) = 0;

    /** some operate interface */

    /** 
     * @brief get the result set of the sql 
     */
    virtual resultset_ptr   query(const char* sql) = 0;
    virtual resultset_ptr   query(const char* sql, error_code& err) = 0;
    virtual resultset_ptr   query(prepared_statement_ptr stmt) = 0;
    virtual resultset_ptr   query(prepared_statement_ptr stmt, error_code& err) = 0;

    /** 
     * @brief return the effects rows
     * return -1 if failed.
     */
    virtual int32_t         execute(const char* sql) = 0;
    virtual int32_t         execute(const char* sql, error_code& err) = 0;
    virtual int32_t         execute(prepared_statement_ptr stmt) = 0;
    virtual int32_t         execute(prepared_statement_ptr stmt, error_code& err) = 0;
};

}

#endif