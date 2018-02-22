/**
 * Light weight log lib for c++.
 * 
 * logger.h
 * 
 * auth: kefengxian
 * email:yanortun@msn.cn
 */

#ifndef _LOG4CPP_LOGGER_H_
#define _LOG4CPP_LOGGER_H_

#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <string>

#include "log_event.h"
#include "log_formatter.h"
#include "log_appender.h"

namespace Log4CPP
{
class Logger;
class LogStream
{
public:
    LogStream(Logger *ptr, const Level level) : _logger_ptr(ptr), _level(level)
    {
    }

public:
    void Flush();
    LogStream& operator<< (LogStream& (*pf)(LogStream&))
    {
        return (*pf)(*this);
    }
    LogStream& operator<< (bool val)
    {
        _log_items.push_back(val?"true":"false");
        return *this;
    }
    LogStream& operator<< (short val)
    {
        _log_items.push_back(std::to_string(val));
        return *this;
    }
    LogStream& operator<< (unsigned short data)
    {
        _log_items.push_back(std::to_string(data));
        return *this;
    }
    LogStream& operator<< (int data)
    {
        _log_items.push_back(std::to_string(data));
        return *this;
    }
    LogStream& operator<< (unsigned int data)
    {
        _log_items.push_back(std::to_string(data));
        return *this;
    }
    LogStream& operator<< (long data)
    {
        _log_items.push_back(std::to_string(data));
        return *this;
    }
    LogStream& operator<< (unsigned long data)
    {
        _log_items.push_back(std::to_string(data));
        return *this;
    }
    LogStream& operator<< (long long data)
    {
        _log_items.push_back(std::to_string(data));
        return *this;
    }
    LogStream& operator<< (unsigned long long data)
    {
        _log_items.push_back(std::to_string(data));
        return *this;
    }
    LogStream& operator<< (float val)
    {
        _log_items.push_back(std::to_string(val));
        return *this;
    }
    LogStream& operator<< (double val)
    {
        _log_items.push_back(std::to_string(val));
        return *this;
    }
    LogStream& operator<< (long double val)
    {
        _log_items.push_back(std::to_string(val));
        return *this;
    }
    LogStream& operator<< (const char* data)
    {
        _log_items.push_back(data);
        return *this;
    }
    LogStream& operator<< (const std::string& data)
    {
        _log_items.push_back(data);
        return *this;
    }
    
private:
    Logger* _logger_ptr{nullptr};
    Level _level;
    std::vector<std::string> _log_items;    
};

inline LogStream& Endl(LogStream& stream)
{
    stream.Flush();
    return stream;
}

class Logger
    : public std::enable_shared_from_this<Logger>
{
    friend class LogStream;
private:
    Logger(const char* name) : _log_name(name), _level(Level::ALL)
    {
    }
public:
    ~Logger()
    {
        //std::cout<< "destruct [" << _log_name << "] logger." << std::endl;
    }

public:
    static std::shared_ptr<Logger> GetLogger(const char* module_name);

    void AddAppender(std::shared_ptr<Appender> appender)
    {
        _log_appender_list.push_back(appender);
    }

    void SetLevel(const Level lev)
    {
        _level = lev;
        if( _level == Level::OFF ){
            for(auto& appender : _log_appender_list)
                appender->Stop();
        }else{
            for(auto& appender : _log_appender_list)
                appender->Restart();
        }
    }

    void Debug(const char* log)
    {
        Append(Level::DEBUG, log);
    }
    void Info(const char* log)
    {
        Append(Level::INFO, log);
    }
    void Warn(const char* log)
    {
        Append(Level::WARN, log);
    }
    void Error(const char* log)
    {
        Append(Level::ERROR, log);
    }
    void Fatal(const char* log)
    {
        Append(Level::FATAL, log);
    }

    LogStream Debug()
    {
        return LogStream(this, Level::DEBUG);
    }
    LogStream Info()
    {
        return LogStream(this, Level::INFO);
    }
    LogStream Warn()
    {
        return LogStream(this, Level::WARN);
    }
    LogStream Error()
    {
        return  LogStream(this, Level::ERROR);
    }
    LogStream Fatal()
    {
        return  LogStream(this, Level::FATAL);
    }

private:
    void Append(const Level level, const char* log)
    {
        // permit this level log.
        if( !Allow(level) )
            return;

        LogEvent e(_log_name.c_str(), level, log);

        for(auto& appender : _log_appender_list)
            appender->Append(e);
    }

    bool Allow(const Level level)
    {
        if( _level == Level::OFF )
            return false;

        if( _level == Level::ALL )
            return true;

        int level_t = static_cast<int>(level);
        int _level_t = static_cast<int>(_level);
        return level_t >= _level_t;
    }

private:
    std::string _log_name;
    
    std::vector<std::shared_ptr<Appender>> _log_appender_list;
    Level _level;
};

class LoggerManager
{
private:
    LoggerManager() = default;

public:
    static LoggerManager& Instance()
    {
        static LoggerManager _instance;
        return _instance;
    }

public:
    void Register(const std::string& logger_name, Logger* logger)
    {
        if( _logger_list.count(logger_name) == 0 )
            _logger_list[logger_name] = logger;
    }

    Logger* Query(const std::string& logger_name)
    {
        return _logger_list.count(logger_name) == 0 ? nullptr : _logger_list[logger_name];
    }

    void Disable(const std::string& logger_name)
    {
        if( _logger_list.count(logger_name) == 1 )
            _logger_list[logger_name]->SetLevel(Level::OFF);
    }

    void ShowLoggers()
    {
        for(auto iter : _logger_list )
            std::cout << "logger: " << iter.first << std::endl;
    }

private:
    std::map<std::string, Logger*> _logger_list;
};

inline std::shared_ptr<Logger> Logger::GetLogger(const char* module_name)
{
    Logger* logger = new Logger(module_name);
    LoggerManager::Instance().Register(module_name, logger);

    return std::shared_ptr<Logger>(logger);
}

inline void LogStream::Flush()
{
    std::string log_line;
    for(const auto& item : _log_items )
        log_line.append(item);

    _logger_ptr->Append(_level, log_line.c_str());
}


}
#endif
