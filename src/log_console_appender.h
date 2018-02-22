/**
 * Light weight log lib for c++.
 * 
 * log_console_appender.h
 * 
 * auth: kefengxian
 * email:yanortun@msn.cn
 */

#ifndef _LOG4CPP_CONSOLE_LOG_APPENDER_H_
#define _LOG4CPP_CONSOLE_LOG_APPENDER_H_

#include <iostream>
#include <memory>

#include "log_appender.h"

namespace Log4CPP
{

class ConsoleAppender
    : public Appender
    , public std::enable_shared_from_this<ConsoleAppender>
{
private:
    ConsoleAppender()
    {
    }
public:
    static std::shared_ptr<ConsoleAppender>& Get()
    {
        static std::shared_ptr<ConsoleAppender> _instance(new ConsoleAppender);
        return _instance;
    }

    ~ConsoleAppender()
    {
        Stop();
    }

private:
    void Output(const std::string& log_str) override
    {
        std::cout << log_str << std::endl;
    }
};

}

#endif
