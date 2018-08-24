/**
 *
 * mysql_prepared_resultset.hpp
 *
 * mysql_prepared_resultset implement
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_mysql_prepared_resultset_hpp__
#define __ydk_dbcp_mysql_prepared_resultset_hpp__

#include <dbcp/driver/mysql/mysql_config.hpp>
#include <dbcp/driver/mysql/mysql_resultset.hpp>
#include <dbcp/driver/mysql/mysql_result_databind.hpp>
#include <dbcp/driver/mysql/mysql_prepared_resultset_metadata.hpp>
#include <dbcp/internal/logger_handler.hpp>

namespace dbcp{
namespace mysql{

class mysql_prepared_resultset : public mysql_resultset
{
protected:
    MYSQL_STMT*                 mysql_stmt_;
    mysql_result_databind*      result_bind_;
public:
    mysql_prepared_resultset(MYSQL_STMT* mysql_stmt)
        : mysql_resultset(nullptr)
        , mysql_stmt_(mysql_stmt)
        , result_bind_(nullptr)
    {
        if (mysql_stmt_){
            meta_data_ = new mysql_prepared_resultset_metadata(mysql_stmt_);
            result_bind_ = new mysql_result_databind(mysql_stmt_, meta_data_);
            int32_t res = result_bind_->bind_result();
            if (res){
                const char* msg = ::mysql_stmt_error(mysql_stmt_);
                dbcp_log_error("mysql_prepared_resultset bind result error[%d], msg[%d].", res, msg);
            }

            num_fields_ = meta_data_->get_column_count();
            num_rows_ = ::mysql_stmt_num_rows(mysql_stmt_);

            for (int32_t i = 0; i < num_fields_; ++i){
                MYSQL_FIELD* field = meta_data_->get_field(i);
                if (field){
                    std::string field_name = field->name;
                    colname_colindex_map_.insert(std::make_pair(field_name, i));
                }
            }
        }
    }

    ~mysql_prepared_resultset(){
        close();

        if (meta_data_){
            delete meta_data_;
            meta_data_ = nullptr;
        }

        if (result_bind_){
            delete result_bind_;
            result_bind_ = nullptr;
        }
    }

public:
    /** interface impls */
    virtual void        close() override{
        if (mysql_stmt_){
            ::mysql_stmt_free_result(mysql_stmt_);
            mysql_stmt_ = nullptr;
        }
    }

    virtual bool        is_closed() override{
        return mysql_stmt_ == nullptr;
    }

    virtual bool        is_null(int32_t col) override{
        if (col < 0 || col >= num_rows_){
            return true;
        }

        if (!result_bind_){
            return true;
        }

        return !!*result_bind_->result_bind_[col].is_null;
    }

    virtual double      get_double(int32_t col) override{
        if (is_before_first() || is_after_last()){
            return 0.0;
        }

        if (col < 0 || col >= num_fields_){
            return 0.0;
        }

        if (!meta_data_){
            return 0.0;
        }

        if (!result_bind_){
            return 0.0;
        }

        if (*result_bind_->result_bind_[col].is_null){
            return 0.0;
        }

        switch (meta_data_->get_column_type(col)){
        case MYSQL_TYPE_BIT:
        case MYSQL_TYPE_YEAR:
        case MYSQL_TYPE_TINY:
        case MYSQL_TYPE_SHORT:
        case MYSQL_TYPE_INT24:
        case MYSQL_TYPE_LONG:
        case MYSQL_TYPE_LONGLONG:
            return static_cast<double>(get_uint64(col));
        case MYSQL_TYPE_TIMESTAMP:
        case MYSQL_TYPE_DATE:
        case MYSQL_TYPE_TIME:
        case MYSQL_TYPE_DATETIME:
        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_BLOB:
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_JSON:
        case MYSQL_TYPE_SET:
        case MYSQL_TYPE_ENUM:
        case MYSQL_TYPE_DECIMAL:
        case MYSQL_TYPE_NEWDECIMAL:
        {
            std::string v = get_string(col);
            std::stringstream ss;
            ss << v;
            double ret = 0.0;
            ss >> ret;
            return ret;
        }
        case MYSQL_TYPE_FLOAT:
        {
            double ret = (double)*reinterpret_cast<float*>(result_bind_->result_bind_[col].buffer);
            return ret;
        }
        case MYSQL_TYPE_DOUBLE:
        {
            double ret = (double)*reinterpret_cast<double*>(result_bind_->result_bind_[col].buffer);
            return ret;
        }
        }
        return 0.0;
    }

    virtual uint64_t    get_uint64(int32_t col) override{
        if (is_before_first() || is_after_last()){
            return 0;
        }

        if (col < 0 || col >= num_fields_){
            return 0;
        }

        if (!meta_data_){
            return 0;
        }

        if (!result_bind_){
            return 0;
        }

        if (*result_bind_->result_bind_[col].is_null){
            return 0;
        }

        switch (meta_data_->get_column_type(col))
        {
        case MYSQL_TYPE_BIT:
            return 0;
        case MYSQL_TYPE_YEAR:
        case MYSQL_TYPE_TINY:
        case MYSQL_TYPE_SHORT:
        case MYSQL_TYPE_INT24:
        case MYSQL_TYPE_LONG:
        case MYSQL_TYPE_LONGLONG:
        {
            uint64_t ret = 0;
            bool is_unsigned = !!result_bind_->result_bind_[col].is_unsigned;
            char* buffer = (char*)result_bind_->result_bind_[col].buffer;
            switch (result_bind_->result_bind_[col].buffer_length)
            {
            case 1:
            {
                ret = (uint64_t)(is_unsigned ? *reinterpret_cast<uint8_t*>(buffer) : *reinterpret_cast<int8_t*>(buffer));
                break;
            }
            case 2:
            {
                ret = (uint64_t)(is_unsigned ? *reinterpret_cast<uint16_t*>(buffer) : *reinterpret_cast<int16_t*>(buffer));
                break;
            }
            case 4:
            {
                ret = (uint64_t)(is_unsigned ? *reinterpret_cast<uint32_t*>(buffer) : *reinterpret_cast<int32_t*>(buffer));
                break;
            }
            case 8:
            {
                ret = (uint64_t)(is_unsigned ? *reinterpret_cast<uint64_t*>(buffer) : *reinterpret_cast<int64_t*>(buffer));
                break;
            }
            default:
                break;
            }
            return ret;
        }
        case MYSQL_TYPE_TIMESTAMP:
        case MYSQL_TYPE_DATE:
        case MYSQL_TYPE_TIME:
        case MYSQL_TYPE_DATETIME:
        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_BLOB:
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_JSON:
        case MYSQL_TYPE_SET:
        case MYSQL_TYPE_ENUM:
        case MYSQL_TYPE_DECIMAL:
        case MYSQL_TYPE_NEWDECIMAL:
        {
            bool is_unsigned = !!result_bind_->result_bind_[col].is_unsigned;
            std::string v = get_string(col);
            if (is_unsigned){
                return strtoull(v.c_str(), NULL, 10);
            }
            return strtoll(v.c_str(), NULL, 10);
        }
        default:
            break;
        }

        return 0;
    }

    virtual std::string get_string(int32_t col) override{
        if (is_before_first() || is_after_last()){
            return "";
        }

        if (col < 0 || col >= num_fields_){
            return "";
        }

        if (!meta_data_){
            return "";
        }

        if (!result_bind_){
            return "";
        }

        if (*result_bind_->result_bind_[col].is_null){
            return "";
        }

        switch (meta_data_->get_column_type(col))
        {
        case MYSQL_TYPE_TIMESTAMP:
        case MYSQL_TYPE_DATETIME:
        {
            char buf[32] = { 0 };
            MYSQL_TIME* t = static_cast<MYSQL_TIME*>(result_bind_->result_bind_[col].buffer);
            if (t->second_part){
                snprintf(buf, sizeof(buf) - 1, "%04d-%02d-%02d %02d:%02d:%02d.%06lu",
                    t->year, t->month, t->day, t->hour, t->minute, t->second, t->second_part);
            }
            else{
                snprintf(buf, sizeof(buf) - 1, "%04d-%02d-%02d %02d:%02d:%02d",
                    t->year, t->month, t->day, t->hour, t->minute, t->second);
            }
            return std::move(std::string(buf));
        }
        case MYSQL_TYPE_DATE:
        {
            char buf[16] = { 0 };
            MYSQL_TIME* t = static_cast<MYSQL_TIME*>(result_bind_->result_bind_[col].buffer);
            snprintf(buf, sizeof(buf) - 1, "%02d-%02d-%02d", t->year, t->month, t->day);
            return std::move(std::string(buf));
        }
        case MYSQL_TYPE_TIME:
        {
            char buf[24] = { 0 };
            MYSQL_TIME* t = static_cast<MYSQL_TIME*>(result_bind_->result_bind_[col].buffer);
            if (t->second_part) {
                snprintf(buf, sizeof(buf), "%s%02d:%02d:%02d.%06lu", t->neg ? "-" : "", t->hour, t->minute, t->second, t->second_part);
            }
            else {
                snprintf(buf, sizeof(buf), "%s%02d:%02d:%02d", t->neg ? "-" : "", t->hour, t->minute, t->second);
            }
            return std::move(std::string(buf));
        }
        case MYSQL_TYPE_TINY_BLOB:
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_BLOB:
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_JSON:
        case MYSQL_TYPE_SET:
        case MYSQL_TYPE_ENUM:
        case MYSQL_TYPE_DECIMAL:
        case MYSQL_TYPE_NEWDECIMAL:
        {
            std::string ret((char*)result_bind_->result_bind_[col].buffer, *result_bind_->result_bind_[col].length);
            return std::move(ret);
        }
        case MYSQL_TYPE_BIT:
        case MYSQL_TYPE_YEAR:
        case MYSQL_TYPE_TINY:
        case MYSQL_TYPE_SHORT:
        case MYSQL_TYPE_INT24:
        case MYSQL_TYPE_LONG:
        case MYSQL_TYPE_LONGLONG:
        {
            char buf[32] = { 0 };
            if (result_bind_->result_bind_[col].is_unsigned){
                uint64_t v = get_uint64(col);
                snprintf(buf, sizeof(buf) - 1, "%llu", v);
            }
            else{
                int64_t v = get_int64(col);
                snprintf(buf, sizeof(buf) - 1, "%lld", v);
            }
            return std::move(std::string(buf));
        }
        case MYSQL_TYPE_FLOAT:
        case MYSQL_TYPE_DOUBLE:
        {
            double v = get_double(col);
            char buf[64] = { 0 };
            snprintf(buf, sizeof(buf) - 1, "%f", v);
            return std::move(std::string(buf));
        }
        default:
            break;
        }

        return "";
    }

protected:
    virtual bool    seek() override{
        if (!mysql_stmt_){
            return false;
        }

        ::mysql_stmt_data_seek(mysql_stmt_, cur_rows_pos_);
        int32_t ret = ::mysql_stmt_fetch(mysql_stmt_);

        if (ret){
            const char* error_msg = ::mysql_stmt_error(mysql_stmt_);
            dbcp_log_error("prepared seek error, no[%d], msg[%s].", ret, error_msg);
        }

        return ret == 0;
    }
};
}
}

#endif
