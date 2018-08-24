/**
 *
 * some global define
 *
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-05
 */

#ifndef __ydk_dbcp_basedefine_hpp__
#define __ydk_dbcp_basedefine_hpp__

#include <memory>

namespace dbcp{
    class statement;
    class prepared_statement;
    class resultset;

    typedef std::shared_ptr<statement>          statement_ptr;
    typedef std::shared_ptr<prepared_statement> prepared_statement_ptr;
    typedef std::shared_ptr<resultset>          resultset_ptr;
}

#endif