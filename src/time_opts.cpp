#include <sys/time.h>
#include <mzx/time_opts.h>

namespace mzx {

int64_t TimeOpts::UnixTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return static_cast<int64_t>(tv.tv_sec) * 1000
        + static_cast<int64_t>(tv.tv_usec) / 1000;
}

}
