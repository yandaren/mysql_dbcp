/**
 *
 * mysql_prepared_resultset_metadata.hpp
 *
 * mysql_prepared_resultset_metadata
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_mysql_prepared_resultset_metadata_hpp__
#define __ydk_dbcp_mysql_prepared_resultset_metadata_hpp__

#include <dbcp/driver/mysql/mysql_config.hpp>
#include <dbcp/driver/mysql/mysql_resultset_metadata.hpp>

namespace dbcp{
namespace mysql{

class mysql_prepared_resultset_metadata : 
    public mysql_resultset_metadata
{
protected:
    MYSQL_STMT* mysql_stmt_;

public:
    mysql_prepared_resultset_metadata(MYSQL_STMT* mysql_stmt)
        : mysql_resultset_metadata(::mysql_stmt_result_metadata(mysql_stmt))
        , mysql_stmt_(mysql_stmt)
    {
    }

    ~mysql_prepared_resultset_metadata(){

    }
};
    
}
}

#endif