#include <mzx/logger.h>
#include <mzx/enum_util.h>
#include <iostream>

namespace mzx {

void LogPrint(Logger::Level level, const char *file_name, int line, const std::string &message)
{
    const static char *level_names[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    std::cerr << "[mzx " << level_names[EnumUtil::ToValue(level)]
        << " " << file_name
        << ":" << line
        << "]" << message
        << std::endl;
}

Logger::Printer Logger::printer_ = LogPrint;

}
