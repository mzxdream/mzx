#ifndef __MZX_LOGGER_H__
#define __MZX_LOGGER_H__

#include <sstream>
#include <functional>
#include <cstdlib>
#include <utility>

namespace mzx {

class Logger
{
public:
    enum class Level
    {
        Debug = 0,
        Info,
        Warn,
        Error,
        Fatal,
    };
    typedef std::function<void (Level, const char *, int, const std::string &)> Printer;
public:
    Logger() = delete;
    ~Logger() = delete;
    Logger(const Logger &) = delete;
    Logger& operator=(const Logger &) = delete;
public:
    static void SetPrinter(const Printer &printer);
    template <typename ...Args>
    static void Print(Level level, const char *file_name, int line, Args && ...args)
    {
        if (printer_)
        {
            std::ostringstream stream;
            ReadArgs(stream, std::forward<Args>(args)...);
            printer_(level, file_name, line, stream.str());
        }
        if (level >= Level::Fatal)
        {
            std::abort();
        }
    }
private:
    template <typename T, typename ...Args>
    static void ReadArgs(std::ostringstream &stream, T &&arg, Args && ...args)
    {
        stream << std::forward<T>(arg);
        ReadArgs(stream, std::forward<Args>(args)...);
    }
    static void ReadArgs(std::ostringstream &stream)
    {
    }
private:
    static Printer printer_;
};

}

#define MZX_LOG(level, args...) mzx::Logger::Print(level, __FILE__, __LINE__, __func__, args)
#define MZX_LOG_IF(level, condition, args...) !(condition) ? (void)0 : MZX_LOG(level, args)

#define MZX_DEBUG(args...) MZX_LOG(mzx::Logger::Level::Debug, args)
#define MZX_DEBUG_IF(args...) MZX_LOG_IF(mzx::Logger::Level::Debug, args)

#define MZX_INFO(args...) MZX_LOG(mzx::Logger::Level::Info, args)
#define MZX_INFO_IF(args...) MZX_LOG_IF(mzx::Logger::Level::Info, args)

#define MZX_WARN(args...) MZX_LOG(mzx::Logger::Level::Warn, args)
#define MZX_WARN_IF(args...) MZX_LOG_IF(mzx::Logger::Level::Warn, args)

#define MZX_ERR(args...) MZX_LOG(mzx::Logger::Level::Error, args)
#define MZX_ERR_IF(args...) MZX_LOG_IF(mzx::Logger::Level::Debug, args)

#define MZX_FATAL(args...) MZX_LOG(mzx::Logger::Level::Debug, args)
#define MZX_FATAL_IF(args...) MZX_LOG_IF(mzx::Logger::Level::Debug, args)

#endif
