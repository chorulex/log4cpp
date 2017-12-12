#include <cstdio>
#include <cstring>
#include <cassert>
#include <errno.h>
#include <sys/signal.h>

#include <string>
#include <vector>
#include <iostream>

#include "../src/log_console_appender.h"
#include "../src/log_file_appender.h"
#include "../src/logger.h"

const char* PROMPT_STR = ">> ";
#define TEST_PROMPT(func) printf("[%s] --- RUNNING\n", func);
#define ERR_PROMPT() printf("line:%d, error:%s.\n", __LINE__, ex.what());

void TestConstructConsoleFormatter()
{
    TEST_PROMPT(__FUNCTION__);

    std::shared_ptr<Log4CPP::Formatter> formatter(new Log4CPP::ConsoleFormatter());
}

void TestConstructFileFormatter()
{
    TEST_PROMPT(__FUNCTION__);

    std::shared_ptr<Log4CPP::Formatter> formatter(new Log4CPP::FileFormatter());
}

void TestConstructConsoleAppender()
{
    TEST_PROMPT(__FUNCTION__);

    std::shared_ptr<Log4CPP::ConsoleAppender> appender = Log4CPP::ConsoleAppender::Get();
}

void TestConstructFileAppender()
{
    TEST_PROMPT(__FUNCTION__);

    const char* file_path = "test.log";
    std::shared_ptr<Log4CPP::FileAppender> appender(new Log4CPP::FileAppender(file_path));
}

void TestConstructLogger()
{
    TEST_PROMPT(__FUNCTION__);

    const char* module = "test";
    std::shared_ptr<Log4CPP::Logger> logger = Log4CPP::Logger::GetLogger(module);
}

void TestInitLogger()
{
    TEST_PROMPT(__FUNCTION__);

    std::shared_ptr<Log4CPP::Formatter> console_formatter(new Log4CPP::ConsoleFormatter());
    std::shared_ptr<Log4CPP::Appender> console_appender = Log4CPP::ConsoleAppender::Get();
    console_appender->SetFormatter(console_formatter);

    const char* file_path = "test.log";
    std::shared_ptr<Log4CPP::Formatter> file_formatter(new Log4CPP::FileFormatter());
    std::shared_ptr<Log4CPP::FileAppender> file_appender(new Log4CPP::FileAppender(file_path));
    file_appender->SetFormatter(file_formatter);

    const char* module = "test";
    std::shared_ptr<Log4CPP::Logger> logger = Log4CPP::Logger::GetLogger(module);
    logger->AddAppender(console_appender);
    logger->AddAppender(file_appender);
}

#include "../src/loghelper.h"
void TestAppendConsoleLog()
{
    TEST_PROMPT(__FUNCTION__);
    std::shared_ptr<Log4CPP::Formatter> console_formatter(new Log4CPP::ConsoleFormatter());
    std::shared_ptr<Log4CPP::Appender> console_appender = Log4CPP::ConsoleAppender::Get();
    console_appender->SetFormatter(console_formatter);

    const char* module = "test";
    std::shared_ptr<Log4CPP::Logger> logger = Log4CPP::Logger::GetLogger(module);
    logger->AddAppender(console_appender);

    LOG_DEBUG(logger, "this debug log.");
    LOG_ERROR(logger, "this error log.");
    LOG_WARN(logger, "this warn log.");
    LOG_INFO(logger, "this info log.");
}
void TestAppendFileLog()
{
    TEST_PROMPT(__FUNCTION__);

    const char* file_path = "test.log";
    std::shared_ptr<Log4CPP::Formatter> file_formatter(new Log4CPP::FileFormatter());
    std::shared_ptr<Log4CPP::FileAppender> file_appender(new Log4CPP::FileAppender(file_path));
    file_appender->SetFormatter(file_formatter);

    const char* module = "test";
    std::shared_ptr<Log4CPP::Logger> logger = Log4CPP::Logger::GetLogger(module);
    logger->AddAppender(file_appender);

    LOG_DEBUG(logger, "this debug log.");
    LOG_ERROR(logger, "this error log.");
    LOG_WARN(logger, "this warn log.");
    LOG_INFO(logger, "this info log.");
}

int main(int argc, char* argv[])
{
    TestConstructConsoleFormatter();
    TestConstructFileFormatter();
    TestConstructConsoleAppender();
    TestConstructFileAppender();
    TestConstructLogger();

    TestInitLogger();
    TestAppendConsoleLog();
    TestAppendFileLog();
    return 0;
}

