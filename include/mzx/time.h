#ifndef __MZX_TIME_H__
#define __MZX_TIME_H__

#include <cstdint>

namespace mzx {

using TimeType = int64_t;

class Time
{
public:
    Time();
    explicit Time(TimeType t);
    ~Time();
public:
    TimeType GetTime() const;
public:
    static TimeType Now();
    static void Sleep(TimeType time);
private:
    TimeType time_{ 0 };
};

}

#endif
