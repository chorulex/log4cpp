/**
 * Light weight log lib for c++.
 * 
 * log_event.h
 * 
 * auth: kefengxian
 * email:yanortun@msn.cn
 */

#ifndef _LOG4CPP_EVENT_H_
#define _LOG4CPP_EVENT_H_

#include <ctime>
#include <sys/time.h>

namespace Log4CPP
{
enum class Level
{
    ALL = 0,    // open all level.
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    OFF         // turn off all level.
};

class LogEvent
{
public:
    LogEvent(const char* module, const Level log_level, const char* log_text) : _module(module), _level(log_level), _text(log_text)
    {
        gettimeofday(&_timestamp, NULL);
        //time(&_timestamp);
    }

    LogEvent(const LogEvent& other) = default;
    LogEvent(const LogEvent&& other)
    {
        this->_module = other._module;
        this->_level = other._level;
        this->_text = std::move(other._text);
        this->_timestamp = other._timestamp;
    }
    LogEvent& operator= (const LogEvent&& other)
    {
        this->_module = other._module;
        this->_level = other._level;
        this->_text = std::move(other._text);
        this->_timestamp = other._timestamp;
        return *this;
    }

    const char* Module() const { return _module;}
    const timeval& Timestamp() const { return _timestamp;}
    const Level& LogLevel() const { return _level;}
    const std::string& Text() const { return _text;}

private:
    const char* _module;
    Level _level;
    std::string _text;

    struct timeval _timestamp;
};

}
#endif
