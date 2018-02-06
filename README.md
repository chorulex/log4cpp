Light weight log lib for c++.

OS version:  only for linux.
C++ version: c++11 or above.

example:
1: normal case
    const char* file_path = "test.log";
    std::shared_ptr<Log4CPP::Formatter> file_formatter(new Log4CPP::FileFormatter);
    std::shared_ptr<Log4CPP::FileAppender> file_appender(new Log4CPP::FileAppender(file_path));
    file_appender->SetFormatter(file_formatter);

    const char* module = "test";
    std::shared_ptr<Log4CPP::Logger> logger = Log4CPP::Logger::GetLogger(module);
    logger->AddAppender(file_appender);
    logger->SetLevel(Log4CPP::Level::ALL);

    logger->Debug("this debug log.");
    logger->Error("this error log.");
    logger->Warn("this warn log.");
    logger->Info("this info log.");
    logger->Fatal("this fatal log.");

2: stream case
    std::shared_ptr<Log4CPP::Formatter> console_formatter(new Log4CPP::ConsoleFormatter);
    std::shared_ptr<Log4CPP::Appender> console_appender = Log4CPP::ConsoleAppender::Get();
    console_appender->SetFormatter(console_formatter);

    const char* module = "test";
    std::shared_ptr<Log4CPP::Logger> logger = Log4CPP::Logger::GetLogger(module);
    logger->AddAppender(console_appender);
    logger->SetLevel(Log4CPP::Level::ALL);

    int times = 0;
    logger->Debug() << "this debug log." << times++ << Log4CPP::Endl;
    logger->Error() << "this error log." << times++ << Log4CPP::Endl;
    logger->Warn() << "this warn log." << times++ << Log4CPP::Endl;
    logger->Info() << "this info log." << times++ << Log4CPP::Endl;
    logger->Fatal() << "this fatal log." << times++ << Log4CPP::Endl;
