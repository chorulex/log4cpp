/**
 * Light weight log lib for c++.
 * 
 * log_configure.h
 * 
 * auth: kefengxian
 * email:yanortun@msn.cn
 */

#ifndef _LOG4CPP_CONFIGURE_H_
#define _LOG4CPP_CONFIGURE_H_

#include <exception>
#include "log_level.h"

namespace Log4CPP
{
class Configure
{
    Configure() = default;

public:
    Configure(const Configure&) = delete;
    Configure& operator=(const Configure&) = delete;

    Configure(const Configure&&) = delete;
    Configure& operator=(const Configure&&) = delete;

    static Configure& Instance()
    {
        static Configure _instance;
        return _instance;
    }

    void SetLowestLevel(const Level& level) { _lowest_level = level; }
    const Level& GetLowestLevel() const { return _lowest_level; }

    void SetBackupCount(unsigned int count) { _log_back_count = count; }
    unsigned int GetBackupCount() const { return _log_back_count; }

    // unit:MB.
    void SetLogFileMaxSize(unsigned int size)
    { 
        if( size == 0 || size > 10 * 1024)
            throw std::invalid_argument("valid range: (0M, 10G].");

        _file_max_size = size; 
    }
    unsigned int GetLogFileMaxSize() const { return _file_max_size; }

private:
    Level _lowest_level = Level::ALL;
    unsigned int _log_back_count = 0;
    unsigned int _file_max_size = 3; // MB
};

}

#endif