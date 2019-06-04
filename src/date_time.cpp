#include <sys/time.h>
#include <unistd.h>

#include <mzx/date_time.h>
#include <mzx/logger.h>

namespace mzx
{

int64_t DateTime::UnixTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return static_cast<int64_t>(tv.tv_sec) * 1000 +
           static_cast<int64_t>(tv.tv_usec) / 1000;
}

void DateTime::Sleep(int64_t delay)
{
    MZX_CHECK(delay > 0);
    usleep(static_cast<unsigned>(delay * 1000));
}

} // namespace mzx
