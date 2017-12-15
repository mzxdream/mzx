#include <iostream>
#include <mzx/logger.h>

namespace mzx {


void PrintLog(const std::string &log)
{
    std::cerr << log << std::endl;
}

LogLevel Logger::level_ = LogLevel::Warn;
Logger::FnPrintLog Logger::log_print_ = PrintLog;

}
