#include <sys/time.h>
#include <mzx/time.h>
#include <unistd.h>
#include <mzx/logger.h>

namespace mzx {

int64_t Time::Now()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return static_cast<int64_t>(tv.tv_sec) * 1000
        + static_cast<int64_t>(tv.tv_usec) / 1000;
}

void Time::Sleep(int64_t time)
{
    MZX_CHECK(time > 0);
    usleep(static_cast<unsigned>(time * 1000));
}

}
