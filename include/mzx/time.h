#ifndef __MZX_TIME_H__
#define __MZX_TIME_H__

#include <cstdint>

namespace mzx
{

class Time
{
public:
    static int64_t Now();
    static void Sleep(int64_t time);
};

} // namespace mzx

#endif
