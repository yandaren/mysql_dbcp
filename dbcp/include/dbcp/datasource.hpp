/**
 *
 * datasource
 *
 * the connection pool
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_datasource_hpp__
#define __ydk_dbcp_datasource_hpp__

#include <cstdint>

namespace dbcp{

class connection;
class datasource
{
protected:
    std::string name_;
    int32_t     id_;
    std::string validation_query_sql_string_;

public:
    datasource() :id_(0), validation_query_sql_string_("select 1"){}
    virtual ~datasource(){}

public:
   /**
    * @brief set/get datasource name
    */
    void        set_name(const std::string& name){
        name_ = name;
    }

    const std::string& get_name() const {
        return name_;
    }

    /** 
     * @brief set/get datasource id 
     */
    void        set_id(int32_t id){
        id_ = id;
    }

    int32_t     get_id() const{
        return id_;
    }

    /** 
     * @brief set/get validation_query_sql_string_
     */
    void        set_validation_query_sql(const char* sql){
        validation_query_sql_string_ = sql;
    }

    const std::string& get_validation_query_sql(){
        return validation_query_sql_string_;
    }

public:

    /** interfaces */

    /** 
     * @brief try get a connection from the datasource
     */
    virtual connection* get_connection() = 0;

    /** 
     * @brief try return the connection to the pool
     */
    virtual void        reclaim(connection* con) = 0;
};
}

#endif