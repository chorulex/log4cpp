#ifndef _QT_LOG_APPENDER_H_
#define _QT_LOG_APPENDER_H_

#include <queue>

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>

#include "log_formatter.h"

namespace Log4CPP
{

class Appender
{
protected:
    Appender() = default;

public:
    virtual ~Appender() {}

    virtual void SetFormatter(std::shared_ptr<Formatter>& formatter) = 0;
    virtual void Output(const std::string& log_str) = 0;

    void Append(const LogEvent& e)
    {
        std::lock_guard<std::mutex> lock(_queue_mtx);
        _log_queue.push(e);
        _queue_cond.notify_all();
    }

protected:
    std::shared_ptr<Formatter> _log_formatter;

    std::atomic_bool _stop {true};
    std::queue<LogEvent> _log_queue;
    std::mutex _queue_mtx;
    std::condition_variable _queue_cond;
    std::thread _log_loop_thread;
};

}
#endif
