/**
 *
 * resultset_metadata.hpp
 *
 * resultset_metadata interface
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_resultset_metadata_hpp__
#define __ydk_dbcp_resultset_metadata_hpp__

#include <cstdint>
#include <string>

namespace dbcp{

class resultset_metadata
{
public:
    virtual ~resultset_metadata() {}

public:
    /** interface */
    virtual uint32_t    get_column_count() = 0;
    virtual std::string get_column_label(int32_t col) = 0;
    virtual std::string get_column_name(int32_t col) = 0;
    virtual std::string get_catalog_name(int32_t col) = 0;
    virtual int32_t     get_column_type(int32_t col) = 0;
    virtual int32_t     get_column_charset(int32_t col) = 0;
    virtual std::string get_schema_name(int32_t col) = 0;
    virtual std::string get_table_name(int32_t col) = 0;
    virtual bool        is_autoincrement(int32_t col) = 0;
    virtual bool        is_nullable(int32_t col) = 0;
    virtual bool        is_numeric(int32_t col) = 0;
    virtual bool        is_signed(int32_t col) = 0;
    virtual bool        is_zerofill(int32_t col) = 0;
};
}

#endif