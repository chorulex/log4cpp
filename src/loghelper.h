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

#include <cstdarg>
#include <cstring>
#include <sstream>

#include <string>
#include <iostream>
#include <thread>

#include "log4cpp.h"

namespace
{
const static int MAX_LOG_BUF_LEN = 4096;

#define LOG(logger, level, format...) {\
    char buffer[MAX_LOG_BUF_LEN] = {0}; \
    sprintf(buffer, "[%s:%d][%s] ", __FILE__, __LINE__, __FUNCTION__); \
    sprintf(buffer + strlen(buffer), format); \
    logger->level(buffer); \
}

#define LOG_DEBUG(logger, format...) LOG(logger, Debug, format)
#define LOG_INFO(logger, format...)  LOG(logger, Info, format)
#define LOG_WARN(logger, format...)  LOG(logger, Warn, format) 
#define LOG_ERROR(logger, format...) LOG(logger, Error, format)
#define LOG_FATAL(logger, format...) LOG(logger, Fatal, format)

}
#endif
