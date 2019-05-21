#ifndef __MZX_LOGGER_H__
#define __MZX_LOGGER_H__

#include <functional>
#include <sstream>

#include <mzx/macro_util.h>

namespace mzx
{

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
    using PrintHandler = std::function<void(Level, const char *, int, const char *, const std::string &)>;
    using FatalHandler = std::function<void()>;

public:
    Logger() = delete;
    ~Logger() = delete;
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

public:
    static void SetPrintHandler(PrintHandler handler);
    static void SetLevel(Level level);
    static void SetFatalHandler(FatalHandler handler);

public:
    template <typename... Args>
    static void Print(Level level, const char *file_name, int line, const char *func_name, Args &&... args)
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
    template <typename T, typename... Args>
    static void ReadArgs(std::ostringstream &stream, T &&arg, Args &&... args)
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

} // namespace mzx

#define MZX_LOG(level, ...) mzx::Logger::Print(level, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define MZX_LOG_IF(level, condition, ...) !(condition) ? (void)0 : MZX_LOG(level, ##__VA_ARGS__)

#define MZX_DEBUG(...) MZX_LOG(mzx::Logger::Level::Debug, ##__VA_ARGS__)
#define MZX_DEBUG_IF(condition, ...) MZX_LOG_IF(mzx::Logger::Level::Debug, condition, ##__VA_ARGS__)

#define MZX_INFO(...) MZX_LOG(mzx::Logger::Level::Info, ##__VA_ARGS__)
#define MZX_INFO_IF(condition, ...) MZX_LOG_IF(mzx::Logger::Level::Info, condition, ##__VA_ARGS__)

#define MZX_WARN(...) MZX_LOG(mzx::Logger::Level::Warn, ##__VA_ARGS__)
#define MZX_WARN_IF(condition, ...) MZX_LOG_IF(mzx::Logger::Level::Warn, condition, ##__VA_ARGS__)

#define MZX_ERR(...) MZX_LOG(mzx::Logger::Level::Error, ##__VA_ARGS__)
#define MZX_ERR_IF(condition, ...) MZX_LOG_IF(mzx::Logger::Level::Error, condition, ##__VA_ARGS__)

#define MZX_FATAL(...) MZX_LOG(mzx::Logger::Level::Fatal, ##__VA_ARGS__)
#define MZX_FATAL_IF(condition, ...) MZX_LOG_IF(mzx::Logger::Level::Fatal, condition, ##__VA_ARGS__)

#define MZX_CHECK(EXPRESSION, ...)                                                                                     \
    MZX_LOG_IF(mzx::Logger::Level::Fatal, !(EXPRESSION), "CHECK failed:" #EXPRESSION " ", ##__VA_ARGS__)

#define MZX_CHECK_STATIC_1(...) static_assert(__VA_ARGS__, #__VA_ARGS__)
#define MZX_CHECK_STATIC_2(...) static_assert(__VA_ARGS__)
#define MZX_CHECK_STATIC(...) MZX_VA_SELECT(MZX_CHECK_STATIC, __VA_ARGS__)

#endif
