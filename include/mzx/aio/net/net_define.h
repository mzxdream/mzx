#ifndef __MZX_NET_DEFINE_H__
#define __MZX_NET_DEFINE_H__

#include <cstdint>
#include <fcntl.h>
#include <netinet/in.h>

#include <mzx/logger.h>

namespace mzx
{

inline bool SetNonBlock(int fd)
{
    MZX_CHECK(fd < 0);
    auto flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    auto ret = fcntl(fd, F_SETFL, flags);
    if (ret == -1)
    {
        MZX_ERR("set non block fd:", fd, " failed");
        return false;
    }
    return true;
}

inline bool SetCloseOnExec(int fd)
{
    MZX_CHECK(fd < 0);
    auto flags = fcntl(fd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    auto ret = fcntl(fd, F_SETFD, flags);
    if (ret == -1)
    {
        MZX_ERR("set close on exec fd:", fd, " failed");
        return false;
    }
    return true;
}

class NetAddress final
{
public:
    explicit NetAddress();
    explicit NetAddress(uint16_t port, const char *addr = nullptr,
                        bool is_ipv6 = false);
    ~NetAddress();

public:
    sockaddr *Address();
    const sockaddr *Address() const;
    socklen_t Length() const;
    bool IsIPv6() const;

private:
    sockaddr_storage address_;
};

} // namespace mzx

#endif
