#ifndef __MZX_LOGGER_H__
#define __MZX_LOGGER_H__

#include <sstream>
#include <functional>

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
    using PrintHandler = std::function<void (Level, const char *, int, const char *, const std::string &)>;
    using FatalHandler = std::function<void ()>;
public:
    Logger() = delete;
    ~Logger() = delete;
    Logger(const Logger &) = delete;
    Logger& operator=(const Logger &) = delete;
public:
    static void SetPrintHandler(PrintHandler handler);
    static void SetLevel(Level level);
    static void SetFatalHandler(FatalHandler handler);
public:
    template <typename ...Args>
    static void Print(Level level, const char *file_name, int line, const char *func_name, Args && ...args)
    {
        if (level >= level_ && print_handler_)
        {
            std::ostringstream stream;
            ReadArgs(stream, std::forward<Args>(args)...);
            print_handler_(level, file_name, line, func_name, stream.str());
        }
        if (level >= Level::Fatal && fatal_handler_)
        {
            fatal_handler_();
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
    static PrintHandler print_handler_;
    static Level level_;
    static FatalHandler fatal_handler_;
};

}

#define MZX_LOG(level, args...) mzx::Logger::Print(level, __FILE__, __LINE__, __func__, args)
#define MZX_LOG_IF(level, condition, args...) !(condition) ? (void)0 : MZX_LOG(level, args)

#define MZX_DEBUG(args...) MZX_LOG(mzx::Logger::Level::Debug, args)
#define MZX_DEBUG_IF(condition, args...) MZX_LOG_IF(mzx::Logger::Level::Debug, condition, args)

#define MZX_INFO(args...) MZX_LOG(mzx::Logger::Level::Info, args)
#define MZX_INFO_IF(condition, args...) MZX_LOG_IF(mzx::Logger::Level::Info, condition, args)

#define MZX_WARN(args...) MZX_LOG(mzx::Logger::Level::Warn, args)
#define MZX_WARN_IF(condition, args...) MZX_LOG_IF(mzx::Logger::Level::Warn, condition, args)

#define MZX_ERR(args...) MZX_LOG(mzx::Logger::Level::Error, args)
#define MZX_ERR_IF(condition, args...) MZX_LOG_IF(mzx::Logger::Level::Error, condition, args)

#define MZX_FATAL(args...) MZX_LOG(mzx::Logger::Level::Fatal, args)
#define MZX_FATAL_IF(condition, args...) MZX_LOG_IF(mzx::Logger::Level::Fatal, condition, args)

#define MZX_CHECK(EXPRESSION, args...) MZX_LOG_IF(mzx::Logger::Level::Fatal, !(EXPRESSION), "CHECK failed:" #EXPRESSION ": ", args)

#endif
