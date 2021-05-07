#ifndef _SLSLOG_INCLUDE_
#define _SLSLOG_INCLUDE_

#include <cstdarg>
#include <stdio.h>

#include "common.hpp"
#include "SLSLock.hpp"

#define SLS_LOG_FATAL     0
#define SLS_LOG_ERROR     1
#define SLS_LOG_WARNING   2
#define SLS_LOG_INFO      3
#define SLS_LOG_DEBUG     4
#define SLS_LOG_TRACE     5
static char const * LOG_LEVEL_NAME[] = {
		"FATAL",
		"ERROR",
		"WARNING",
		"INFO",
		"DEBUG",
		"TRACE"
};


static const char APP_NAME[] = "SLS";

#define sls_log CSLSLog::log
#define sls_set_log_level CSLSLog::set_log_level
#define sls_set_log_file CSLSLog::set_log_file
/**
 * CSLSLog
 */
class CSLSLog
{
private :
    CSLSLog();
    ~CSLSLog();

public :
    static int  create_instance();
    static int  destory_instance();
    static void log(int level, const char *fmt, ...);
    static void set_log_level(char *level);
    static void set_log_file(char * file_name);

private:
    static CSLSLog* m_pInstance;
    int             m_level;
    CSLSMutex       m_mutex;
    char            log_filename[1024];
    FILE          * m_log_file;

    void print_log(int level, const char *fmt, va_list vl);
};



#endif
