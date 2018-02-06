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

#include "log_appender.h"

namespace Log4CPP
{

class ConsoleAppender : public Appender
{
private:
    ConsoleAppender()
    {
    }
public:
    static std::shared_ptr<Appender>& Get()
    {
        static std::shared_ptr<Appender> _instance(new ConsoleAppender);
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
