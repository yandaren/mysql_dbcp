/**
 *
 * transaction.hpp
 *
 * transaction interface
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2018-08-23
 */

#ifndef __ydk_dbcp_transaction_hpp__
#define __ydk_dbcp_transaction_hpp__

#include <vector>
#include <dbcp/basedefine.hpp>
#include <dbcp/error_code.hpp>
#include <dbcp/connection.hpp>
#include <dbcp/connection_wrapper.hpp>
#include <dbcp/internal/logger_handler.hpp>

namespace dbcp{
    class transaction 
    {
    protected:
        std::vector<prepared_statement_ptr> to_execuate_op_list_;
        connection_wrapper& con_;

    public:
        transaction(connection_wrapper& con): con_(con){
        }

        virtual ~transaction(){}

    public:
        void         execute(prepared_statement_ptr stmt) {
            to_execuate_op_list_.push_back(stmt);
        }

        bool         commit() {
            // set auto_commit(false)
            if (!con_->setautocommit(false)) {
                dbcp_log_error("transaction commit failed, set auth commit to false failed");
                return false;
            }

            for (auto stmt : to_execuate_op_list_) {
                error_code err;

                con_->execute(stmt, err);
                if (err) {
                    dbcp_log_error("transaction commit, error[%d:%s].", err.value(), err.message().c_str());

                    con_->rollback();

                    return false;
                }
            }

            // commit
            if (!con_->commit()) {
                dbcp_log_error("transaction commit failed, try do commit failed");

                con_->rollback();

                return false;
            }

            // set auto_commit(true)
            if (!con_->setautocommit(true)) {
                dbcp_log_error("transaction commit failed, set auth commit to true failed");
            }

            return true;
        }
    };
}

#endif