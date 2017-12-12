#ifndef _QT_LOG_H_
#define _QT_LOG_H_

#include <iostream>
#include <memory>
#include <map>
#include <string>

#include "log_event.h"
#include "log_formatter.h"
#include "log_appender.h"

namespace Log4CPP
{
class Logger
{
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

private:
    void Append(const Level level, const char* log)
    {
        // permit this level log.
        if( !Allow(level) )
            return;

        LogEvent e(_log_name.c_str(), level, log);

        for(auto appender : _log_appender_list)
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

std::shared_ptr<Logger> Logger::GetLogger(const char* module_name)
{
    Logger* logger = new Logger(module_name);
    LoggerManager::Instance().Register(module_name, logger);

    return std::shared_ptr<Logger>(logger);
}

}

#endif
