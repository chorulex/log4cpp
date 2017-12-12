#ifndef _QT_LOG_EVENT_H_
#define _QT_LOG_EVENT_H_

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
