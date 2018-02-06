#ifndef _QT_LOG_APPENDER_H_
#define _QT_LOG_APPENDER_H_

#include <queue>
#include <list>

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>

#include "log_formatter.h"

namespace Log4CPP
{

class Appender
{
    friend class Logger;

protected:
    Appender() = default;

public:
    virtual ~Appender() {}

    virtual void SetFormatter(const std::shared_ptr<Log4CPP::Formatter>& formatter)
    {
        std::lock_guard<std::mutex> lock(_queue_mtx);
        _log_formatter = formatter;
    }

    void Append(const LogEvent& e)
    {
        if( !_stop ){
            std::lock_guard<std::mutex> lock(_queue_mtx);
            _log_queue.push_back(std::move(e));
            _queue_cond.notify_all();
        }
    }

protected:
    void stop()
    {
        _stop = true;

        {
            std::lock_guard<std::mutex> lock(_queue_mtx);
            _queue_cond.notify_all();
        }

        if( _log_loop_thread.joinable() )
            _log_loop_thread.join();

    }

    void start()
    {
        _stop = false;
        _log_loop_thread = std::thread(&Appender::WriteLogThread, this);
        while( !_thread_exec );
    }

    void restart()
    {
        stop();
        start();
    }

protected:
    virtual void WriteLogThread() = 0;
    virtual void Output(const std::string& log_str) = 0;

protected:
    std::shared_ptr<Log4CPP::Formatter> _log_formatter;

    std::list<LogEvent> _log_queue;
    std::mutex _queue_mtx;
    std::condition_variable _queue_cond;

    std::atomic_bool _stop {true};
    std::thread _log_loop_thread;
    std::atomic_bool _thread_exec {false};
};

}
#endif
