#ifndef __MZX_TIME_UTIL_H__
#define __MZX_TIME_UTIL_H__

#include <cstdint>

namespace mzx {

class TimeUtil
{
public:
    static int64_t Now();
    static void Sleep(int64_t ms);
};

}

#endif
