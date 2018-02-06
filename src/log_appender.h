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
class Work
{
public:
    Work(Appender* appender) : _appender(appender)
    {
    }

    void Post(const LogEvent& e)
    {
        if( !_stop ){
            std::lock_guard<std::mutex> lock(_queue_mtx);
            _log_queue.push_back(std::move(e));
            _queue_cond.notify_all();
        }
    }

    void Stop()
    {
        _stop = true;

        {
            std::lock_guard<std::mutex> lock(_queue_mtx);
            _queue_cond.notify_all();
        }

        if( _log_loop_thread.joinable() )
            _log_loop_thread.join();

    }

    void Start()
    {
        _stop = false;
        _log_loop_thread = std::thread(&Work::WriteLogThread, this);
        while( !_thread_exec );
    }

    void Restart()
    {
        Stop();
        Start();
    }

private:
    void WriteLogThread()
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

        _appender->Output(_appender->Format(log_ev));
    }

private:
    std::list<LogEvent> _log_queue;
    std::mutex _queue_mtx;
    std::condition_variable _queue_cond;

    std::atomic_bool _stop {true};
    std::thread _log_loop_thread;
    std::atomic_bool _thread_exec {false};

    Appender* _appender{nullptr};
};

    friend class Logger;
protected:
    Appender() : _worker(this)
    {
    }

public:
    virtual ~Appender() {}

    virtual void SetFormatter(const std::shared_ptr<Log4CPP::Formatter>& formatter)
    {
        _log_formatter = formatter;
    }

    void Append(const LogEvent& e)
    {
        _worker.Post(e);
    }

protected:
    void Stop()
    {
        _worker.Stop();
    }

    void Start()
    {
        _worker.Start();
    }

    void Restart()
    {
        _worker.Restart();
    }

protected:
    virtual void Output(const std::string& log_str) = 0;

private:
    std::string Format(const LogEvent& log_ev)
    {
        return std::move(_log_formatter->Format(log_ev));
    }

private:
    Work _worker;

    std::shared_ptr<Log4CPP::Formatter> _log_formatter;
};

}
#endif
