#include <cstdio>
#include <cstring>
#include <cassert>
#include <errno.h>
#include <sys/signal.h>
#include <typeinfo>

#include <chrono>
#include <string>
#include <vector>
#include <iostream>

#include "log4cpp.h"
#include "loghelper.h"

const char* PROMPT_STR = ">> ";
#define TEST_PROMPT(func) printf("[%s] --- RUNNING\n", func);
#define ERR_PROMPT() printf("line:%d, error:%s.\n", __LINE__, ex.what());

void TestConstructConsoleFormatter()
{
    TEST_PROMPT(__FUNCTION__);

    Log4CPP::LoggerManager::Instance().Clear();

    std::shared_ptr<Log4CPP::ConsoleFormatter> formatter(new Log4CPP::ConsoleFormatter());
    auto new_app = formatter->shared_from_this();
    assert(new_app != nullptr);
}

void TestConstructFileFormatter()
{
    TEST_PROMPT(__FUNCTION__);

    Log4CPP::LoggerManager::Instance().Clear();

    std::shared_ptr<Log4CPP::FileFormatter> formatter(new Log4CPP::FileFormatter());
    auto new_app = formatter->shared_from_this();
    assert(new_app != nullptr);
}

void TestConstructConsoleAppender()
{
    TEST_PROMPT(__FUNCTION__);

    Log4CPP::LoggerManager::Instance().Clear();

    std::shared_ptr<Log4CPP::ConsoleAppender> appender = Log4CPP::ConsoleAppender::Get();
    auto new_app = appender->shared_from_this();
    assert(new_app != nullptr);
}

void TestConstructFileAppender()
{
    TEST_PROMPT(__FUNCTION__);

    Log4CPP::LoggerManager::Instance().Clear();

    const char* file_path = "test.log";
    std::shared_ptr<Log4CPP::FileAppender> appender(new Log4CPP::FileAppender(file_path));
    auto new_app = appender->shared_from_this();
    assert(new_app != nullptr);
}

void TestConstructLogger()
{
    TEST_PROMPT(__FUNCTION__);

    Log4CPP::LoggerManager::Instance().Clear();

    const char* module = "test";
    std::shared_ptr<Log4CPP::Logger> logger = Log4CPP::Logger::GetLogger(module);
    auto new_app = logger->shared_from_this();
    assert(new_app != nullptr);
}

void TestInitLogger()
{
    TEST_PROMPT(__FUNCTION__);

    Log4CPP::LoggerManager::Instance().Clear();

    std::shared_ptr<Log4CPP::Formatter> console_formatter(new Log4CPP::ConsoleFormatter);
    std::shared_ptr<Log4CPP::ConsoleAppender> console_appender = Log4CPP::ConsoleAppender::Get();
    console_appender->SetFormatter(console_formatter);

    const char* file_path = "test.log";
    std::shared_ptr<Log4CPP::Formatter> file_formatter(new Log4CPP::FileFormatter);
    std::shared_ptr<Log4CPP::FileAppender> file_appender(new Log4CPP::FileAppender(file_path));
    file_appender->SetFormatter(file_formatter);

    const char* module = "test";
    std::shared_ptr<Log4CPP::Logger> logger = Log4CPP::Logger::GetLogger(module);
    logger->AddAppender(console_appender);
    logger->AddAppender(file_appender);
}

void TestAppendConsoleLog()
{
    TEST_PROMPT(__FUNCTION__);

    Log4CPP::LoggerManager::Instance().Clear();
    Log4CPP::Configure::Instance().SetLowestLevel(Log4CPP::Level::ALL);

    std::shared_ptr<Log4CPP::Formatter> console_formatter(new Log4CPP::ConsoleFormatter);
    std::shared_ptr<Log4CPP::ConsoleAppender> console_appender = Log4CPP::ConsoleAppender::Get();
    console_appender->SetFormatter(console_formatter);
    console_appender->Start();

    const char* module = "test";
    std::shared_ptr<Log4CPP::Logger> logger = Log4CPP::Logger::GetLogger(module);
    logger->AddAppender(console_appender);

    logger->Debug("this debug log.");
    logger->Error("this error log.");
    logger->Warn("this warn log.");
    logger->Info("this info log.");
    logger->Fatal("this fatal log.");

    console_appender->Stop();
}
void TestAppendFileLog()
{
    TEST_PROMPT(__FUNCTION__);

    Log4CPP::Configure::Instance().SetLowestLevel(Log4CPP::Level::ALL);
    Log4CPP::LoggerManager::Instance().Clear();

    const char* file_path = "test.log";
    std::shared_ptr<Log4CPP::Formatter> file_formatter(new Log4CPP::FileFormatter);
    std::shared_ptr<Log4CPP::FileAppender> file_appender(new Log4CPP::FileAppender(file_path));
    file_appender->SetFormatter(file_formatter);
    file_appender->Start();

    const char* module = "test";
    std::shared_ptr<Log4CPP::Logger> logger = Log4CPP::Logger::GetLogger(module);
    logger->AddAppender(file_appender);

    logger->Debug("this debug log.");
    logger->Error("this error log.");
    logger->Warn("this warn log.");
    logger->Info("this info log.");
    logger->Fatal("this fatal log.");
}
void TestAppendConsoleLogByStream()
{
    TEST_PROMPT(__FUNCTION__);

    Log4CPP::Configure::Instance().SetLowestLevel(Log4CPP::Level::ALL);
    Log4CPP::LoggerManager::Instance().Clear();

    std::shared_ptr<Log4CPP::Formatter> console_formatter(new Log4CPP::ConsoleFormatter);
    std::shared_ptr<Log4CPP::ConsoleAppender> console_appender = Log4CPP::ConsoleAppender::Get();
    console_appender->SetFormatter(console_formatter);
    console_appender->Start();

    const char* module = "test";
    std::shared_ptr<Log4CPP::Logger> logger = Log4CPP::Logger::GetLogger(module);
    logger->AddAppender(console_appender);

    int times = 0;
    logger->Debug() << "this debug log." << times++ << Log4CPP::Endl;
    logger->Error() << "this error log." << times++ << Log4CPP::Endl;
    logger->Warn() << "this warn log." << times++ << Log4CPP::Endl;
    logger->Info() << "this info log." << times++ << Log4CPP::Endl;
    logger->Fatal() << "this fatal log." << times++ << Log4CPP::Endl;

    console_appender->Stop();
}

#define _TEST_LOG_STREAM_TYPE(var) \
    logger->Debug() << "this debug log. "<<#var<<"(type:" << typeid(var).name() << ")" << var << Log4CPP::Endl;
void TestAppendConsoleLogByStreamAllType()
{
    TEST_PROMPT(__FUNCTION__);

    Log4CPP::LoggerManager::Instance().Clear();
    Log4CPP::Configure::Instance().SetLowestLevel(Log4CPP::Level::ALL);

    std::shared_ptr<Log4CPP::Formatter> console_formatter(new Log4CPP::ConsoleFormatter);
    std::shared_ptr<Log4CPP::ConsoleAppender> console_appender = Log4CPP::ConsoleAppender::Get();
    console_appender->SetFormatter(console_formatter);
    console_appender->Start();

    const char* module = "test";
    std::shared_ptr<Log4CPP::Logger> logger = Log4CPP::Logger::GetLogger(module);
    logger->AddAppender(console_appender);

    const std::string log_tex("this fatal log.");
    logger->Fatal() << log_tex << Log4CPP::Endl;

    bool val1 = false;
    _TEST_LOG_STREAM_TYPE(val1);
    val1 = true;
    _TEST_LOG_STREAM_TYPE(val1);

    short val2 = -1;
    _TEST_LOG_STREAM_TYPE(val2);

    unsigned short val3 = 0;
    _TEST_LOG_STREAM_TYPE(val3);

    int val4 = -1;
    _TEST_LOG_STREAM_TYPE(val4);

    unsigned int val5 = 1;
    _TEST_LOG_STREAM_TYPE(val5);

    long val6 = -1l;
    _TEST_LOG_STREAM_TYPE(val6);

    unsigned long val7 = 1lu;
    _TEST_LOG_STREAM_TYPE(val7);

    long long val8 = -1ll;
    _TEST_LOG_STREAM_TYPE(val8);

    unsigned long long val9 = 1llu;
    _TEST_LOG_STREAM_TYPE(val9);

    float val10 = 0.1f;
    _TEST_LOG_STREAM_TYPE(val10);

    double val11 = 0.1;
    _TEST_LOG_STREAM_TYPE(val11);

    long double val12 = 0.1;
    _TEST_LOG_STREAM_TYPE(val12);

    char *val13 = new char[6];
    sprintf(val13, "hello");
    _TEST_LOG_STREAM_TYPE(val13);
    delete [] val13;

    console_appender->Stop();
}

void TestConfigure()
{
    TEST_PROMPT(__FUNCTION__);

    Log4CPP::Configure& cfg = Log4CPP::Configure::Instance();

    assert(Log4CPP::CurrentWorkDirectory().compare(cfg.GetDirectory()) == 0);
    const char* log_dir = "./";
    cfg.SetDirectory(log_dir);
    assert(::strcmp(cfg.GetDirectory(), log_dir) == 0);

    Log4CPP::Level level = Log4CPP::Level::DEBUG;
    assert(cfg.GetLowestLevel() == Log4CPP::Level::ALL);
    cfg.SetLowestLevel(level);
    assert(cfg.GetLowestLevel() == level);

    unsigned int count = 10;
    assert(cfg.GetBackupCount() == 0);
    cfg.SetBackupCount(count);
    assert(cfg.GetBackupCount() == count);

    unsigned int file_size = 1; //MB
    assert(cfg.GetLogFileMaxSize() == 3);
    cfg.SetLogFileMaxSize(file_size);
    assert(cfg.GetLogFileMaxSize() == file_size);

    try{
        cfg.SetLogFileMaxSize(0);
        assert(false);
    }catch(std::invalid_argument ex){
    }
}

void TestHelper()
{
    TEST_PROMPT(__FUNCTION__);

    Log4CPP::Configure::Instance().SetLowestLevel(Log4CPP::Level::ALL);
    Log4CPP::LoggerManager::Instance().Clear();

    std::shared_ptr<Log4CPP::Formatter> console_formatter(new Log4CPP::ConsoleFormatter);
    std::shared_ptr<Log4CPP::ConsoleAppender> console_appender = Log4CPP::ConsoleAppender::Get();
    console_appender->SetFormatter(console_formatter);
    console_appender->Start();

    const char* module = "test";
    std::shared_ptr<Log4CPP::Logger> logger = Log4CPP::Logger::GetLogger(module);
    logger->AddAppender(console_appender);

    int times = 0;
    LOG_DEBUG(logger, "this debug log %d." ,times++);
    LOG_ERROR(logger, "this error log %d." ,times++);
    LOG_WARN(logger,  "this warn log %d."  ,times++);
    LOG_INFO(logger,  "this info log %d."  ,times++);
    LOG_FATAL(logger, "this fatal log %d." ,times++);

    console_appender->Stop();
}

int main(int argc, char* argv[])
{
    TestConfigure();

    TestInitLogger();
    TestConstructConsoleFormatter();
    TestConstructFileFormatter();
    TestConstructConsoleAppender();
    TestConstructFileAppender();
    TestConstructLogger();

    TestAppendFileLog();

    TestAppendConsoleLog();
    TestAppendConsoleLogByStream();
    TestAppendConsoleLogByStreamAllType();

    TestHelper();
    return 0;
}

