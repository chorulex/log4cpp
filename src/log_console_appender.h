#ifndef _QT_CONSOLE_LOG_APPENDER_H_
#define _QT_CONSOLE_LOG_APPENDER_H_

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
        stop();
    }

private:
    void WriteLogThread() override
    {
        _thread_exec = true;

        while(!_stop){
            std::unique_lock<std::mutex> lock(_queue_mtx);
            _queue_cond.wait(lock, [this]{ return !_log_queue.empty() || _stop;});

            if( _stop ) break;
            Pop();
        }

        while( !_log_queue.empty() ) Pop();
        _thread_exec = false;
    }

    void Pop()
    {
        LogEvent log_ev = std::move(_log_queue.front());
        _log_queue.pop_front();

        const std::string& log_str = _log_formatter->Format(log_ev);
        Output(log_str);
    }
    void Output(const std::string& log_str) override
    {
        std::cout << log_str << std::endl;
    }
};

}

#endif
