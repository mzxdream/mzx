#ifndef __MZX_TIME_H__
#define __MZX_TIME_H__

#include <cstdint>

namespace mzx
{

class DateTime
{
public:
    static int64_t UnixTime();
    static void Sleep(int64_t delay);
};

} // namespace mzx

#endif
