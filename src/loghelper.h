/**
 * Light weight log lib for c++.
 * 
 * loghepler.h
 * 
 * auth: kefengxian
 * email:yanortun@msn.cn
 */

#ifndef _LOG4CPP_HELPER_H_
#define _LOG4CPP_HELPER_H_

#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <cstdarg>
#include <cstring>
#include <sstream>

#include <string>
#include <iostream>
#include <thread>

#include "log_file_appender.h"
#include "log_console_appender.h"

#include "logger.h"

namespace
{
unsigned int GetCurrentThreadID()
{
    return syscall(SYS_gettid);
}

const static int MAX_LOG_BUF_LEN = 4096;

#define LOG_LOCATION() \
    sprintf(buffer, "[%u] [%s:%d][%s] ", GetCurrentThreadID(), __FILE__, __LINE__, __FUNCTION__); 

#define LOG_DEBUG(logger, format, ...) \
{ \
    char buffer[MAX_LOG_BUF_LEN] = {0}; \
    LOG_LOCATION();\
    sprintf(buffer + strlen(buffer), format,  ##__VA_ARGS__); \
    logger->Debug(buffer); \
}

#define LOG_INFO(logger, format, ...) \
{ \
    char buffer[MAX_LOG_BUF_LEN] = {0}; \
    LOG_LOCATION();\
    sprintf(buffer + strlen(buffer), format,  ##__VA_ARGS__); \
    logger->Info(buffer); \
}

#define LOG_WARN(logger, format, ...) \
{ \
    char buffer[MAX_LOG_BUF_LEN] = {0}; \
    LOG_LOCATION();\
    sprintf(buffer + strlen(buffer), format,  ##__VA_ARGS__); \
    logger->Warn(buffer); \
}

#define LOG_ERROR(logger, format, ...) \
{ \
    char buffer[MAX_LOG_BUF_LEN] = {0}; \
    LOG_LOCATION();\
    sprintf(buffer + strlen(buffer), format,  ##__VA_ARGS__); \
    logger->Error(buffer); \
}

#define LOG_FATAL(logger, format, ...) \
{ \
    char buffer[MAX_LOG_BUF_LEN] = {0}; \
    LOG_LOCATION();\
    sprintf(buffer + strlen(buffer), format,  ##__VA_ARGS__); \
    logger->Fatal(buffer); \
}

}
#endif
