/**
 * Light weight log lib for c++ like log4j
 * 
 * log4cpp.h
 * 
 * auth: kefengxian
 * email:yanortun@msn.cn
 */

#ifndef _LOG4CPP_LOGGER_H_
#define _LOG4CPP_LOGGER_H_

// linux
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

// C++ std
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <list>
#include <thread>

#include <iostream>
#include <fstream>

#include <exception>
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace Log4CPP
{
// begin namespace

std::string CurrentWorkDirectory()
{
    std::string current_directory("./");
    char* c_current_dir = get_current_dir_name();
    if( c_current_dir != NULL ){
        current_directory = c_current_dir;
        free( c_current_dir );
    }

    return current_directory;
}

enum class Level : int
{
    ALL = 0,    // open all level.
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    OFF         // turn off all level.
};

class LogEvent
{
public:
    LogEvent(const char* module, const Level log_level, const char* log_text) : _module(module), _level(log_level), _text(log_text)
    {
        gettimeofday(&_timestamp, NULL);
        //time(&_timestamp);
    }

    LogEvent(const LogEvent& other) = default;
    LogEvent(const LogEvent&& other)
    {
        this->_module = other._module;
        this->_level = other._level;
        this->_text = std::move(other._text);
        this->_timestamp = other._timestamp;
    }
    LogEvent& operator= (const LogEvent&& other)
    {
        this->_module = other._module;
        this->_level = other._level;
        this->_text = std::move(other._text);
        this->_timestamp = other._timestamp;
        return *this;
    }

    const char* Module() const { return _module;}
    const timeval& Timestamp() const { return _timestamp;}
    const Level& LogLevel() const { return _level;}
    const std::string& Text() const { return _text;}

private:
    const char* _module;
    Level _level;
    std::string _text;

    struct timeval _timestamp;
};

/**
 * log global Configure
 * 
 * config param:
 * 1: lowset level
 * 2: back up log file count
 * 3: max log file size unit MB.
 * 
 * NOTE:
 * Set configure on first.
 * 
 */
class Configure
{
    Configure()
    {
        _work_dir = CurrentWorkDirectory();
    }

public:
    Configure(const Configure&) = delete;
    Configure& operator=(const Configure&) = delete;

    Configure(const Configure&&) = delete;
    Configure& operator=(const Configure&&) = delete;

    static Configure& Instance()
    {
        static Configure _instance;
        return _instance;
    }

    const char* GetDirectory() const { return _work_dir.c_str(); }
    void SetDirectory(const char* dir) { _work_dir.assign(dir); }

    void SetLowestLevel(const Level& level) { _lowest_level = level; }
    const Level& GetLowestLevel() const { return _lowest_level; }

    void SetBackupCount(unsigned int count) { _log_back_count = count; }
    unsigned int GetBackupCount() const { return _log_back_count; }

    // unit:MB.
    void SetLogFileMaxSize(unsigned int size)
    { 
        if( size == 0 || size > 10 * 1024)
            throw std::invalid_argument("valid range: (0M, 10G].");

        _file_max_size = size; 
    }
    unsigned int GetLogFileMaxSize() const { return _file_max_size; }

private:
    std::string _work_dir;

    Level _lowest_level = Level::ALL;
    unsigned int _log_back_count = 0;
    unsigned int _file_max_size = 3; // MB
};

class Formatter
{
protected:
    virtual ~Formatter() {};

public:
    std::string Format(const LogEvent& e)
    {
        std::string log_str = std::move(this->FormatHeader(e));
        log_str.append(e.Text());

        return std::move(log_str);
    }

protected:
    virtual std::string FormatHeader(const LogEvent& e) = 0;

    std::string FormatTimestamp(const timeval& tv)
    {
        time_t _now = tv.tv_sec;
        struct tm _tm_now;
        localtime_r(&_now, &_tm_now);

        char time_str[32] = {0};
        size_t len = strftime(time_str, sizeof(time_str), "%Y%m%d-%H:%M:%S", &_tm_now);
        sprintf(time_str + len, ".%03d", (int)(tv.tv_usec/1000));

        return time_str;
    }

    const char* FormatLevel(Level level) const
    {
        switch(level)
        {
        case Level::DEBUG: return "[DEBUG]";
        case Level::INFO:  return "[INFO] ";
        case Level::WARN:  return "[WARN] ";
        case Level::ERROR: return "[ERROR]";
        case Level::FATAL: return "[FATAL]";
        default: 
            return "[UNKNOWN]";
        }

        return "";
    }
};

class ConsoleFormatter
    : public Formatter
    , public std::enable_shared_from_this<ConsoleFormatter>
{
public:
    ~ConsoleFormatter() {}

public:
private:
    std::string FormatHeader(const LogEvent& e) override
    {
        std::string header;
        header.append("[").append(FormatTimestamp(e.Timestamp())).append("] ");
        header.append("[").append(e.Module()).append("] ");
        header.append(FormatLevel(e.LogLevel())).append(" ");
        return std::move(header);
    }
};

class FileFormatter
    : public Formatter
    , public std::enable_shared_from_this<FileFormatter>
{
public:
private:
    std::string FormatHeader(const LogEvent& e) override
    {
        std::string header;
        header.append("[").append(std::move(FormatTimestamp(e.Timestamp()))).append("] ");
        header.append(FormatLevel(e.LogLevel())).append(" ");
        return std::move(header);
    }
};

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

public:
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
        //std::cout << log_str << std::endl;
        printf("%s\n", log_str.c_str());
    }
};

class FileAppender
    : public Appender
    , public std::enable_shared_from_this<FileAppender>
{
public:
    /**
     * file_path:
     * file_count: max log file count.
     * max_size_per_file: max size on MB per log file.
     */
    FileAppender(const char* file_path)
    {
        _file_path.assign(Configure::Instance().GetDirectory());
        _file_path.append(file_path);

        Open();
    }
    ~FileAppender()
    {
        Stop();
        Close();
    }

private:
    bool Open()
    {
        _file_buffer.open(_file_path, std::ios::app);
        if( !_file_buffer ){
            std::cerr << "fail to open file " << _file_path << std::endl;
            return false;
        }

        return true;
    }
    void Close()
    {
        if( _file_buffer ){
            _file_buffer.flush();
            _file_buffer.clear();
            _file_buffer.close();
        }
    }

    void Output(const std::string& log_str) override
    {
        //std::cout << log_str << std::endl;
        _file_buffer << log_str << std::endl;

        if( IsFull() ){
            Close();
            Backup();

            Open();
        }
    }

    bool IsFull()
    {
        if( Configure::Instance().GetBackupCount() ==  0 )
            return false;

        return static_cast<unsigned long>(_file_buffer.tellp()) >= Configure::Instance().GetLogFileMaxSize() * 1024 * 1024;
    }

    void Backup()
    {
        for(int index = Configure::Instance().GetBackupCount(); index > 0; index--){
            const std::string& src_log_file = ConstructLogFilePath(index - 1);
            const std::string& des_log_file = ConstructLogFilePath(index);

            // delete old bak log file.
            remove(des_log_file.c_str());

            struct stat file_stat;
            if( stat(src_log_file.c_str(), &file_stat ) == 0 ){
                rename(src_log_file.c_str(), des_log_file.c_str());
            }
        }

        // delete primary log file.
        remove(_file_path.c_str());
    }

    std::string ConstructLogFilePath(int index)
    {
        std::string file_path(_file_path);
        if( index > 0 )
            file_path.append(".").append(std::to_string(index));

        return file_path;
    }

private:
    std::ofstream _file_buffer;
    std::string _file_path;
};


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
    Logger(const char* name) : _log_name(name)
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
        const Level& lowest_level = Configure::Instance().GetLowestLevel();
        if( lowest_level == Level::OFF )
            return false;

        if( lowest_level == Level::ALL )
            return true;

        int level_t = static_cast<int>(level);
        int _level_t = static_cast<int>(lowest_level);
        return level_t >= _level_t;
    }

private:
    std::string _log_name;
    
    std::vector<std::shared_ptr<Appender>> _log_appender_list;
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
    void Clear()
    {
        _logger_list.clear();
    }

    void Register(const std::string& logger_name, std::shared_ptr<Logger> logger)
    {
        if( _logger_list.count(logger_name) == 0 )
            _logger_list[logger_name] = logger;
    }

    std::shared_ptr<Logger> Query(const std::string& logger_name) const
    {
        auto iter = _logger_list.find(logger_name);
        if( iter == _logger_list.end() )
            return std::shared_ptr<Logger>();

        return iter->second;
    }

    void ShowLoggers()
    {
        for(auto iter : _logger_list )
            std::cout << "logger: " << iter.first << std::endl;
    }

private:
    std::map<std::string, std::shared_ptr<Logger>> _logger_list;
};

inline std::shared_ptr<Logger> Logger::GetLogger(const char* module_name)
{
    LoggerManager& manager = LoggerManager::Instance();

    std::shared_ptr<Logger> logger = manager.Query(module_name);
    if( !logger ){
        logger.reset(new Logger(module_name));
        manager.Register(module_name, logger);
    }

    return logger;
}

inline void LogStream::Flush()
{
    std::string log_line;
    for(const auto& item : _log_items )
        log_line.append(item);

    _logger_ptr->Append(_level, log_line.c_str());
}

} // end namespace
#endif
