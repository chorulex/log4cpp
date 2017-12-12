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
        //std::cout << "construct ConsoleAppender" << std::endl;
        _stop = false;
        _log_loop_thread = std::thread([this]{
            while(!_stop){
                std::unique_lock<std::mutex> lock(_queue_mtx);
                _queue_cond.wait(lock, [this]{ return !_log_queue.empty() || _stop;});
                if( _stop )
                    break;

                Pop();
            }

            while( !_log_queue.empty() )
                Pop();
        });
    }
public:
    static std::shared_ptr<ConsoleAppender>& Get()
    {
        static std::shared_ptr<ConsoleAppender> _instance(new ConsoleAppender);
        return _instance;
    }

    ~ConsoleAppender()
    {
        _stop = true;
        _queue_cond.notify_all();

        if( _log_loop_thread.joinable() )
            _log_loop_thread.join();
    }

    void SetFormatter(std::shared_ptr<Formatter>& formatter) override
    {
        std::lock_guard<std::mutex> lock(_queue_mtx);
        _log_formatter = formatter;
    }

    void Output(const std::string& log_str) override
    {
        std::cout << log_str << std::endl;
    }

private:
    void Pop()
    {
        LogEvent log_ev = _log_queue.front();
        _log_queue.pop();

        const std::string& log_str = _log_formatter->Format(log_ev);
        Output(log_str);
    }
};

}

#endif
