#ifndef _LOGGER_H_
#define _LOGGER_H_

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
std::string GetCurrentThreadID()
{
    std::ostringstream stream;
    stream << std::this_thread::get_id();
    return stream.str();
}

const static int MAX_LOG_BUF_LEN = 4096;

#define LOG_DEBUG(logger, format, ...) \
{ \
    char buffer[MAX_LOG_BUF_LEN] = {0}; \
    sprintf(buffer, "[%s] [%s:%d][%s] ", GetCurrentThreadID().c_str(), __FILE__, __LINE__, __FUNCTION__); \
    sprintf(buffer + strlen(buffer), format,  ##__VA_ARGS__); \
    logger->Debug(buffer); \
}

#define LOG_INFO(logger, format, ...) \
{ \
    char buffer[MAX_LOG_BUF_LEN] = {0}; \
    sprintf(buffer, "[%s] [%s:%d][%s] ", GetCurrentThreadID().c_str(), __FILE__, __LINE__, __FUNCTION__); \
    sprintf(buffer + strlen(buffer), format,  ##__VA_ARGS__); \
    logger->Info(buffer); \
}

#define LOG_WARN(logger, format, ...) \
{ \
    char buffer[MAX_LOG_BUF_LEN] = {0}; \
    sprintf(buffer, "[%s] [%s:%d][%s] ", GetCurrentThreadID().c_str(), __FILE__, __LINE__, __FUNCTION__); \
    sprintf(buffer + strlen(buffer), format,  ##__VA_ARGS__); \
    logger->Warn(buffer); \
}

#define LOG_ERROR(logger, format, ...) \
{ \
    char buffer[MAX_LOG_BUF_LEN] = {0}; \
    sprintf(buffer, "[%s] [%s:%d][%s] ", GetCurrentThreadID().c_str(), __FILE__, __LINE__, __FUNCTION__); \
    sprintf(buffer + strlen(buffer), format,  ##__VA_ARGS__); \
    logger->Error(buffer); \
}

#define LOG_FATAL(logger, format, ...) \
{ \
    char buffer[MAX_LOG_BUF_LEN] = {0}; \
    sprintf(buffer, "[%s] [%s:%d][%s] ", GetCurrentThreadID().c_str(), __FILE__, __LINE__, __FUNCTION__); \
    sprintf(buffer + strlen(buffer), format,  ##__VA_ARGS__); \
    logger->Fatal(buffer); \
}

}
#endif
