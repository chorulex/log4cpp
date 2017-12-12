#ifndef _QT_FILE_LOG_APPENDER_H_
#define _QT_FILE_LOG_APPENDER_H_

#include <sys/stat.h>
#include <iostream>
#include <fstream>

#include "log_appender.h"

namespace Log4CPP
{

class FileAppender : public Appender
{
public:
    /**
     * file_path:
     * file_count: max log file count.
     * max_size_per_file: max size on MB per log file.
     */
    FileAppender(const char* file_path, int file_count = 1, int max_size_per_file = 10)
        : _file_path(file_path)
        , _max_log_file_count(file_count)
        , _max_file_size(max_size_per_file * 1024 * 1024)
    {
        if( !Open() )
            return ;

        _stop = false;
        _log_loop_thread = std::thread(&FileAppender::WriteLogThread, this);
    }
    ~FileAppender()
    {
        _stop = true;
        _queue_cond.notify_all();

        if( _log_loop_thread.joinable() )
            _log_loop_thread.join();

        Close();
    }

    void SetFormatter(std::shared_ptr<Formatter>& formatter) override
    {
        _log_formatter = formatter;
    }

    void Output(const std::string& log_str) override
    {
        _file_buffer << log_str << std::endl;
    }

private:
    void WriteLogThread()
    {
        while(!_stop){
            std::unique_lock<std::mutex> lock(_queue_mtx);
            _queue_cond.wait(lock, [this]{ return !_log_queue.empty() || _stop;});
            if( _stop ){
                while( !_log_queue.empty() ) Pop();
                break;
            }

            if( !Pop() )
                break;
        }
    }
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

    bool Pop()
    {
        LogEvent log_ev = _log_queue.front();
        _log_queue.pop();

        const std::string& log_str = _log_formatter->Format(log_ev);
        Output(log_str);

        if( IsFull() ){
            Close();
            Backup();

            return Open();
        }

        return true;
    }

    bool IsFull()
    {
        if( _max_log_file_count ==  1 )
            return false;

        return static_cast<unsigned long>(_file_buffer.tellp()) >= _max_file_size;
    }

    void Backup()
    {
        for(int index = _max_log_file_count; index > 0; index--){
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

    const std::string _file_path;
    int _max_log_file_count = 1;
    unsigned long _max_file_size = 0;
};

};
#endif
