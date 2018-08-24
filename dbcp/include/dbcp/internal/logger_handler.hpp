/**
 *
 * logger_handler.hpp
 *
 * dbcp mysql internal logger handler
 *
 * @author  :   yandaren1220@126.com
 * @date    :   2017-08-04
 */

#ifndef __ydk_dbcp_internal_log_handler_hpp__
#define __ydk_dbcp_internal_log_handler_hpp__

#include <functional>
#include <chrono>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#ifdef _WIN32
#define snprintf _snprintf
#endif

namespace dbcp
{
namespace internal
{

enum dbcp_log_level
{
    dbcp_log_level_debug = 0,
    dbcp_log_level_info = 1,
    dbcp_log_level_warn = 2,
    dbcp_log_level_error = 3,
    dbcp_log_level_fatal = 4,
    dbcp_log_level_none = 5,
};

/**
* @brief logger handler
*/
typedef std::function<void(dbcp_log_level log_level,
    const char* data, int32_t len)> log_handler_func;

class log_handler
{
public:
    log_handler()
    {
        log_lvl_ = dbcp_log_level_debug;
        log_handler_func_ = log_handler::defualt_log_handler;
    }

    ~log_handler(){}

    /** not thread safe */
    static log_handler& instance()
    {
        static log_handler s_log_handler;
        return s_log_handler;
    }

public:
    void    set_log_handler(const log_handler_func& func)
    {
        log_handler_func_ = func;
    }

    void    set_log_lvl(dbcp_log_level lvl)
    {
        log_lvl_ = lvl;
    }

    dbcp_log_level get_log_lvl()
    {
        return log_lvl_;
    }

    void    log_msg(dbcp_log_level log_level, const char* data, int32_t len)
    {
        (log_handler_func_)(log_level, data, len);
    }

protected:
    static const char* get_loglvl_str(dbcp_log_level lvl){
        static const char* log_level_strs[] = { "debug", "info", "warn", "error", "fatal" };
        static const char* uknow_level = "unknow";
        if (lvl >= dbcp_log_level_debug &&
            lvl <= dbcp_log_level_fatal){
            return log_level_strs[lvl];
        }
        return uknow_level;
    }
    static void defualt_log_handler(dbcp_log_level log_level, const char* data, int32_t len)
    {
        auto tp = std::chrono::system_clock::now();
        auto tt = std::chrono::system_clock::to_time_t(tp);
        auto duration = tp.time_since_epoch();
        int32_t millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;

        struct tm* ptm = localtime(&tt);
        char date[64] = { 0 };

        sprintf(date, "%d-%02d-%02d %02d:%02d:%02d,%03d",
            (int32_t)ptm->tm_year + 1900, (int32_t)ptm->tm_mon + 1, (int32_t)ptm->tm_mday,
            (int32_t)ptm->tm_hour, (int32_t)ptm->tm_min, (int32_t)ptm->tm_sec, millis);

        printf("[%s][%s][db] %s\n", date, get_loglvl_str(log_level), data);
    }

protected:
    log_handler_func log_handler_func_;
    dbcp_log_level   log_lvl_;
};

static void dbcp_log_msg(dbcp_log_level log_lvl, const char* format, ...){
    log_handler& log_h = log_handler::instance();
    if (log_lvl < log_h.get_log_lvl())
        return;

#define max_log_len 1023
    char buffer[max_log_len + 1];
    char* log_buff = buffer;
    va_list ap;
    va_start(ap, format);
    int32_t len = vsnprintf(0, 0, format, ap);
    va_end(ap);

    if (len > max_log_len){
        log_buff = (char*)malloc(len + 1);
    }
    va_start(ap, format);
    len = vsprintf(log_buff, format, ap);
    va_end(ap);

    log_h.log_msg(log_lvl, log_buff, len);

    if (log_buff != buffer){
        free(log_buff);
    }
#undef max_log_len
}

/**
* @brief set the custom log handler
*/
static void set_log_handler(const log_handler_func& func){
    log_handler& log_h = log_handler::instance();
    log_h.set_log_handler(func);
}

/**
* @brief set the log level
*/
static void set_log_lvl(dbcp_log_level lvl){
    log_handler& log_h = log_handler::instance();
    log_h.set_log_lvl(lvl);
}

/**
* @brief the static logger_handler initialize not thread safe, in muti thread enviroment,
* you need call the logger handler initialize function first.
*/
static void logger_initialize(){
    log_handler& log_h = log_handler::instance();
    dbcp_log_msg(dbcp_log_level_info, "dbcp_logger_initialize.");
}
}
}

#define dbcp_log dbcp::internal::dbcp_log_msg
#define dbcp_log_debug(...) dbcp_log(dbcp::internal::dbcp_log_level_debug, __VA_ARGS__)
#define dbcp_log_info(...) dbcp_log(dbcp::internal::dbcp_log_level_info, __VA_ARGS__)
#define dbcp_log_warn(...) dbcp_log(dbcp::internal::dbcp_log_level_warn, __VA_ARGS__)
#define dbcp_log_error(...) dbcp_log(dbcp::internal::dbcp_log_level_error, __VA_ARGS__)
#define dbcp_log_fatal(...) dbcp_log(dbcp::internal::dbcp_log_level_fatal,  __VA_ARGS__)

#endif