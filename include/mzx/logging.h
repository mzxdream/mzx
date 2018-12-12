#ifndef __MZX_LOGGING_H__
#define __MZX_LOGGING_H__

namespace mzx {

enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
};

class LogMessage
{
public:
    LogMessage(LogLevel level, const char *file_name, int line, const char *func_name);
    ~LogMessage();
public:
    LogMessage & operator<<();
};


}

#endif
