/**
 *
 * mysql_resultset_metadata.hpp
 *
 * mysql_resultset_metadata
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_mysql_resultset_metadata_hpp__
#define __ydk_dbcp_mysql_resultset_metadata_hpp__

#include <dbcp/driver/mysql/mysql_config.hpp>
#include <dbcp/resultset_metadata.hpp>

namespace dbcp{
namespace mysql{

class mysql_resultset_metadata : public resultset_metadata
{
public:
    friend class mysql_resultset;
    friend class mysql_result_databind;
    friend class mysql_prepared_resultset;

protected:
    int32_t         num_field_;
    MYSQL_FIELD*    fields_;

public:
    mysql_resultset_metadata(MYSQL_RES*  res)
        : num_field_(0)
        , fields_(nullptr)
    {
        if (res){
            num_field_ = ::mysql_num_fields(res);
            fields_ = ::mysql_fetch_fields(res);
        }
    }

    virtual ~mysql_resultset_metadata(){
    }

public:
    /** interface */
    virtual uint32_t    get_column_count() override{
        return num_field_;
    }

    virtual std::string get_column_label(int32_t col) override{
        MYSQL_FIELD* field = get_field(col);
        if (field){
            return std::move(std::string(field->name, field->name_length));
        }
        return "";
    }

    virtual std::string get_column_name(int32_t col) override{
        MYSQL_FIELD* field = get_field(col);
        if (field){
            return std::move(std::string(field->org_name, field->org_name_length));
        }
        return "";
    }

    virtual std::string get_catalog_name(int32_t col) override{
        MYSQL_FIELD* field = get_field(col);
        if (field){
            return std::move(std::string(field->catalog, field->catalog_length));
        }
        return "";
    }

    virtual int32_t     get_column_type(int32_t col) override{
        MYSQL_FIELD* field = get_field(col);
        if (field){
            return field->type;
        }
        return 0;
    }

    virtual int32_t     get_column_charset(int32_t col) override{
        MYSQL_FIELD* field = get_field(col);
        if (field){
            return field->charsetnr;
        }
        return 0;
    }

    virtual std::string get_schema_name(int32_t col) override{
        MYSQL_FIELD* field = get_field(col);
        if (field){
            return std::move(std::string(field->db, field->db_length));
        }
        return "";
    }

    virtual std::string get_table_name(int32_t col) override{
        MYSQL_FIELD* field = get_field(col);
        if (field){
            return std::move(std::string(field->org_table, field->org_table_length));
        }
        return "";
    }

    virtual bool        is_autoincrement(int32_t col) override{
        MYSQL_FIELD* field = get_field(col);
        if (field){
            return (field->flags & AUTO_INCREMENT_FLAG) != 0;
        }
        return false;
    }

    virtual bool        is_nullable(int32_t col) override{
        MYSQL_FIELD* field = get_field(col);
        if (field){
            return (field->flags & NOT_NULL_FLAG) != 0;
        }
        return false;
    }

    virtual bool        is_numeric(int32_t col) override{
        MYSQL_FIELD* field = get_field(col);
        if (field){
            return (field->flags & NUM_FLAG) != 0;
        }
        return false;
    }

    virtual bool        is_signed(int32_t col) override{
        MYSQL_FIELD* field = get_field(col);
        if (field){
            if (field->type == FIELD_TYPE_YEAR){
                return false;
            }
            return !(field->flags & UNSIGNED_FLAG);
        }
        return false;
    }

    virtual bool        is_zerofill(int32_t col) override{
        MYSQL_FIELD* field = get_field(col);
        if (field){
            return (field->flags & ZEROFILL_FLAG) != 0;
        }
        return false;
    }

protected:
    virtual MYSQL_FIELD*    get_field(int32_t col){
        if (col >= 0 && col < num_field_){
            return &fields_[col];
        }
        return nullptr;
    }
};
}
}

#endif