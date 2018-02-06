#ifndef _QT_LOG_FORMATTER_H_
#define _QT_LOG_FORMATTER_H_

#include <string>

#include "log_event.h"

namespace Log4CPP
{

class Formatter
{
protected:
    virtual ~Formatter() {};

public:
    std::string Format(const LogEvent& e)
    {
        std::string log_str = std::move(this->FormatHeader(e));
        log_str.append(e.Text());

        return std::move(log_str);
    }

protected:
    virtual std::string FormatHeader(const LogEvent& e) = 0;

    std::string FormatTimestamp(const timeval& tv)
    {
        time_t _now = tv.tv_sec;
        struct tm _tm_now;
        localtime_r(&_now, &_tm_now);

        char time_str[32] = {0};
        size_t len = strftime(time_str, sizeof(time_str), "%Y%m%d-%H:%M:%S", &_tm_now);
        sprintf(time_str + len, ".%03d", (int)(tv.tv_usec/1000));

        return time_str;
    }

    const char* FormatLevel(Level level)
    {
        switch(level)
        {
        case Level::DEBUG: return "[DEBUG]";
        case Level::INFO:  return "[INFO] ";
        case Level::WARN:  return "[WARN] ";
        case Level::ERROR: return "[ERROR]";
        case Level::FATAL: return "[FATAL]";
        default: 
            return "[UNKNOWN]";
        }

        return "";
    }
};

class ConsoleFormatter : public Formatter
{
public:
    ~ConsoleFormatter() {}

public:
private:
    std::string FormatHeader(const LogEvent& e) override
    {
        std::string header;
        header.append("[").append(FormatTimestamp(e.Timestamp())).append("] ");
        header.append("[").append(e.Module()).append("] ");
        header.append(FormatLevel(e.LogLevel())).append(" ");
        return std::move(header);
    }
};
class FileFormatter : public Formatter
{
public:
private:
    std::string FormatHeader(const LogEvent& e) override
    {
        std::string header;
        header.append("[").append(std::move(FormatTimestamp(e.Timestamp()))).append("] ");
        header.append(FormatLevel(e.LogLevel())).append(" ");
        return std::move(header);
    }
};

}

#endif
