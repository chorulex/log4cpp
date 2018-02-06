/**
 * Light weight log lib for c++.
 * 
 * log_file_appender.h
 * 
 * auth: kefengxian
 * email:yanortun@msn.cn
 */

#ifndef _LOG4CPP_FILE_LOG_APPENDER_H_
#define _LOG4CPP_FILE_LOG_APPENDER_H_

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
        std::cout << log_str << std::endl;
        _file_buffer << log_str << std::endl;

        if( IsFull() ){
            Close();
            Backup();

            Open();
        }
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
