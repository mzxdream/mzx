#include <mzx/logger.h>
#include <mzx/enum_util.h>
#include <cstdio>
#include <cstdlib>
#include <utility>

namespace mzx {

static void LogPrint(Logger::Level level, const char *file_name, int line, const char *func_name, const std::string &message)
{
    const static char *level_names[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    fprintf(stderr, "[mzx %s %s:%d %s] %s\n"
        , level_names[EnumToValue(level)], file_name, line, func_name, message.c_str());
    fflush(stderr);
}

static void LogFatal()
{
    std::abort();
}

Logger::PrintHandler Logger::print_handler_ = LogPrint;
Logger::Level Logger::level_ = Logger::Level::Warn;
Logger::FatalHandler Logger::fatal_handler_ = LogFatal;

void Logger::SetPrintHandler(Logger::PrintHandler handler)
{
    print_handler_ = handler;
}

void Logger::SetLevel(Logger::Level level)
{
    level_ = level;
}

void Logger::SetFatalHandler(Logger::FatalHandler handler)
{
    fatal_handler_ = handler;
}

}
