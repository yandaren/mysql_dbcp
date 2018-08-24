/**
 *
 * resultset.hpp
 *
 * resultset interface
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_resultset_hpp__
#define __ydk_dbcp_resultset_hpp__

namespace dbcp{

class resultset_metadata;
class resultset
{
public:
    virtual ~resultset() {}

public:

    /** interfaces */
    virtual void        close() = 0;
    virtual bool        is_closed() = 0;
    virtual bool        is_first() = 0;
    virtual bool        is_before_first() = 0;
    virtual void        before_first() = 0;
    virtual bool        is_last() = 0;
    virtual bool        is_after_last() = 0;
    virtual void        after_last() = 0;
    virtual bool        move_first() = 0;
    virtual bool        move_last() = 0;
    virtual bool        move_next() = 0;
    virtual bool        move_previous() = 0;
    virtual bool        move_to(int32_t row_index) = 0;
    virtual bool        is_null(int32_t col) = 0;
    virtual bool        is_null(const std::string& col_name) = 0;
    virtual bool        get_boolean(int32_t col) = 0;
    virtual bool        get_boolean(const std::string& col_name) = 0;
    virtual double      get_double(int32_t col) = 0;
    virtual double      get_double(const std::string& col_name) = 0;
    virtual int32_t     get_int32(int32_t col) = 0;
    virtual int32_t     get_int32(const std::string& col_name) = 0;
    virtual uint32_t    get_uint32(int32_t col) = 0;
    virtual uint32_t    get_uint32(const std::string& col_name) = 0;
    virtual int64_t     get_int64(int32_t col) = 0;
    virtual int64_t     get_int64(const std::string& col_name) = 0;
    virtual uint64_t    get_uint64(int32_t col) = 0;
    virtual uint64_t    get_uint64(const std::string& col_name) = 0;
    virtual std::string get_string(int32_t col) = 0;
    virtual std::string get_string(const std::string& col_name) = 0;
    virtual std::string get_blob(int32_t col) = 0;
    virtual std::string get_blob(const std::string& col_name) = 0;
    virtual int32_t     find_column(const std::string& col_name) = 0;
    virtual int64_t     row_count() = 0;
    virtual resultset_metadata* meta_data() = 0;

};
}

#endif