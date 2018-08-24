/**
*
* mysql_resultset.hpp
*
* mysql_resultset implement
*
* @author  :   yandaren1220@126.com
* @date    :   2017-08-05
*/

#ifndef __ydk_dbcp_mysql_resultset_hpp__
#define __ydk_dbcp_mysql_resultset_hpp__

#include <dbcp/driver/mysql/mysql_config.hpp>
#include <dbcp/driver/mysql/mysql_resultset_metadata.hpp>
#include <dbcp/resultset.hpp>
#include <unordered_map>

namespace dbcp{
namespace mysql{

class mysql_resultset : public resultset
{
protected:
    MYSQL_RES* res_;                    // result
    MYSQL_ROW  row_;                    // current row
    int32_t    num_fields_;             // num of fields
    int64_t    num_rows_;               // num of rows_
    int64_t    cur_rows_pos_;           // current row positon
    mysql_resultset_metadata* meta_data_;    // the result meta_data_
    std::unordered_map<std::string, int32_t>  colname_colindex_map_;

public:
    mysql_resultset(MYSQL_RES* res)
        : res_(res)
        , row_(0)
        , num_fields_(0)
        , num_rows_(0)
        , cur_rows_pos_(-1)
        , meta_data_(nullptr)
    {
        if (res_){
            num_fields_ = ::mysql_num_fields(res);
            num_rows_ = ::mysql_num_rows(res);
            meta_data_ = new mysql_resultset_metadata(res_);

            for (int32_t i = 0; i < num_fields_; ++i){
                MYSQL_FIELD* field = meta_data_->get_field(i);
                if (field){
                    std::string field_name = field->name;
                    colname_colindex_map_.insert(std::make_pair(field_name, i));
                }
            }
        }
    }

    ~mysql_resultset(){
        if (meta_data_){
            delete meta_data_;
            meta_data_ = nullptr;
        }

        if (res_){
            ::mysql_free_result(res_);
            res_ = nullptr;
        }
    }

public:

    /** interfaces */
    virtual void        close() override{
        res_ = nullptr;
    }

    virtual bool        is_closed() override{
        return res_ == nullptr;
    }

    virtual bool        is_first() override{
        return cur_rows_pos_ == 0;
    }

    virtual bool        is_before_first() override{
        return cur_rows_pos_ == -1;
    }

    virtual void        before_first() override{
        cur_rows_pos_ = -1;
    }

    virtual bool        is_last() override{
        return cur_rows_pos_ == num_rows_ - 1;
    }

    virtual bool        is_after_last() override{
        return cur_rows_pos_ == num_rows_;
    }

    virtual void        after_last() override{
        cur_rows_pos_ = num_rows_;
    }

    virtual bool        move_first() override{
        if (num_rows_){
            cur_rows_pos_ = 0;
            return seek();
        }

        return false;
    }

    virtual bool        move_last() override{
        if (num_rows_){
            cur_rows_pos_ = num_rows_ - 1;
            return seek();
        }
        return false;
    }

    virtual bool        move_next() override{
        if (is_last()){
            after_last();
        }
        else if (cur_rows_pos_ + 1 < num_rows_){
            cur_rows_pos_++;
            return seek();
        }
        return false;
    }

    virtual bool        move_previous() override{
        if (is_first()){
            before_first();
        }
        else if (cur_rows_pos_ - 1 >= 0){
            cur_rows_pos_--;
            return seek();
        }
        return false;
    }

    virtual bool        move_to(int32_t row_index) override{
        if (row_index >= 0 && row_index < num_rows_){
            cur_rows_pos_ = row_index;
            return seek();
        }
        return false;
    }

    virtual bool        is_null(int32_t col) override{
        if (col < 0 || col >= num_rows_){
            return true;
        }

        if (!row_){
            return true;
        }

        return row_[col] == NULL;
    }

    virtual bool        is_null(const std::string& col_name) override{
        return is_null(find_column(col_name));
    }

    virtual bool        get_boolean(int32_t col) override{
        return !!get_int32(col);
    }

    virtual bool        get_boolean(const std::string& col_name) override{
        return get_boolean(find_column(col_name));
    }

    virtual double      get_double(int32_t col) override{
        if (is_before_first() || is_after_last()){
            return 0.0;
        }

        if (col < 0 || col >= num_fields_){
            return 0.0;
        }

        if (!row_ || !row_[col]){
            return 0.0;
        }

        double ret = 0;
        std::stringstream ss;
        ss << row_[col];
        ss >> ret;
        return ret;
    }

    virtual double      get_double(const std::string& col_name) override{
        return get_double(find_column(col_name));
    }

    virtual int32_t     get_int32(int32_t col) override{
        return static_cast<int32_t>(get_int64(col));
    }

    virtual int32_t     get_int32(const std::string& col_name) override{
        return get_int32(find_column(col_name));
    }

    virtual uint32_t    get_uint32(int32_t col) override{
        return static_cast<uint32_t>(get_uint64(col));
    }

    virtual uint32_t    get_uint32(const std::string& col_name) override{
        return get_uint32(find_column(col_name));
    }

    virtual int64_t     get_int64(int32_t col) override{
        return static_cast<int64_t>(get_uint64(col));
    }

    virtual int64_t     get_int64(const std::string& col_name) override{
        return get_int64(find_column(col_name));
    }

    virtual uint64_t    get_uint64(int32_t col) override{
        if (is_before_first() || is_after_last()){
            return 0;
        }

        if (col < 0 || col >= num_fields_){
            return 0;
        }

        if (!row_ || !row_[col]){
            return 0;
        }

        if (!meta_data_){
            return 0;
        }

        MYSQL_FIELD* field = meta_data_->get_field(col);
        if (!field){
            return 0;
        }

        if (field->flags & UNSIGNED_FLAG){
            return strtoull(row_[col], NULL, 10);
        }
        return strtoll(row_[col], NULL, 10);
    }

    virtual uint64_t    get_uint64(const std::string& col_name) override{
        return get_uint64(find_column(col_name));
    }

    virtual std::string get_string(int32_t col) override{
        if (is_before_first() || is_after_last()){
            return "";
        }

        if (col < 0 || col >= num_fields_){
            return "";
        }

        if (!row_ || !row_[col]){
            return "";
        }

        ulong* lens = ::mysql_fetch_lengths(res_);
        return std::move(std::string(row_[col], lens[col]));
    }

    virtual std::string get_string(const std::string& col_name) override{
        return std::move(get_string(find_column(col_name)));
    }

    virtual std::string get_blob(int32_t col) override{
        return std::move(get_string(col));
    }

    virtual std::string get_blob(const std::string& col_name) override{
        return std::move(get_blob(find_column(col_name)));
    }

    virtual int32_t     find_column(const std::string& col_name) override{
        auto iter = colname_colindex_map_.find(col_name);
        if (iter != colname_colindex_map_.end()){
            return iter->second;
        }
        return -1;
    }

    virtual int64_t     row_count() override{
        return num_rows_;
    }

    virtual resultset_metadata* meta_data() override{
        return meta_data_;
    }

protected:
    virtual bool    seek(){
        if (!res_){
            return false;
        }

        ::mysql_data_seek(res_, cur_rows_pos_);
        row_ = ::mysql_fetch_row(res_);

        return row_ != nullptr;
    }

};
}
}

#endif