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

    virtual void SetFormatter(std::shared_ptr<Formatter>& formatter)
    {
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

    virtual void Output(const std::string& log_str) = 0;

protected:
    void stop()
    {
        _stop = true;
        _queue_cond.notify_all();

        if( _log_loop_thread.joinable() )
            _log_loop_thread.join();
    }

    void start()
    {
        _stop = false;
        _log_loop_thread = std::thread(&Appender::WriteLogThread, this);
    }

    void restart()
    {
        stop();
        start();
    }

protected:
    virtual void WriteLogThread() = 0;

protected:
    std::shared_ptr<Formatter> _log_formatter;

    std::list<LogEvent> _log_queue;
    std::mutex _queue_mtx;
    std::condition_variable _queue_cond;

    std::atomic_bool _stop {true};
    std::thread _log_loop_thread;
};

}
#endif
