#ifndef _M_LOGGER_H_
#define _M_LOGGER_H_

#include <iostream>
#include <mutex>
#include <mzx/util/m_singleton.h>

#define MLOG(logger, level, args...) logger.Print(level, __FILE__, __LINE__, args)
#define MLOG_IF(condition, logger, level, args...) !(condition) ? (void)0 : MLOG(logger, level, args)

#define MINFO  0
#define MWARN  1
#define MERR   2
#define MFATAL 3

class MDefaultLogger
{
public:
    MDefaultLogger()
        :level_filter_(MWARN)
    {
    }
    ~MDefaultLogger();
    MDefaultLogger(const MDefaultLogger &) = delete;
    MDefaultLogger& operator=(const MDefaultLogger &) = delete;
public:
    void SetLevelFilter(int level)
    {
        level_filter_ = level;
    }
    template <typename ...Args>
    void Print(int level, const char *p_file_name, int line, const Args& ...args)
    {
        if (level < level_filter_)
        {
            return;
        }
        std::lock_guard<std::mutex> lock(mtx_);
        std::cerr << "[" << level << " " << p_file_name << ":" << line << "]";
        PrintArgs(args...);
    }
    template <typename T, typename ...Args>
    void PrintArgs(const T &arg, const Args& ...args)
    {
        std::cerr << " " << arg;
        PrintArgs(args...);
    }
    void PrintArgs()
    {
        std::cerr << "\n";
    }
private:
    std::mutex mtx_;
    int level_filter_;
};

inline MDefaultLogger& MGetDefaultLogger()
{
    return MSingleton<MDefaultLogger, std::mutex>::Instance();
}

#endif
