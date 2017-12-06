#ifndef __MZX_LOGGER_H__
#define __MZX_LOGGER_H__

#include <iostream>
#include <sstream>
#include <functional>

#include <mzx/enum_util.h>

namespace mzx {

enum class LoggerLevel
    : int
{
    All,
    Debug,
    Info,
    Warn,
    Error,
    Fatal,
    Off,
};

void PrintLog(const std::string &log)
{
    std::cerr << log << std::endl;
}

class Logger
{
 public:
  typedef std::function<void (const std::string &)> FnPrintLog;
public:
    Logger() = delete;
    ~Logger() = delete;
    Logger(const Logger &) = delete;
    Logger& operator=(const Logger &) = delete;
public:
    static void SetLevel(LoggerLevel level)
    {
        level_ = level;
    }
    template <typename ...Args>
    static void Print(LoggerLevel level, const char *file_name, int line, const Args& ...args)
    {
        if (Enum2Value(level) < Enum2Value(level_))
        {
            return;
        }
        std::ostringstream stream;
        stream << "[" << Enum2Value(level) << " " << file_name << ":" << line << "]";
        PrintArgs(stream, args...);
    }
private:
    template <typename T, typename ...Args>
    static void PrintArgs(std::ostringstream &stream, const T &arg, const Args& ...args)
    {
        stream << " " << arg;
        PrintArgs(stream, args...);
    }
    static void PrintArgs(std::ostringstream &stream)
    {
        stream << "\n";
        if (log_print_)
        {
            log_print_(stream.str());
        }
    }
private:
    static LoggerLevel level_;
    static FnPrintLog log_print_;
};

}

#define MLOG(level, args...) logger.Print(level, __FILE__, __LINE__, __func__, args)
#define MLOG_IF(level, condition, args...) !(condition) ? (void)0 : MLOG(level, args)

#endif
