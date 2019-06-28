#ifndef __MZX_NET_DEFINE_H__
#define __MZX_NET_DEFINE_H__

#include <cstdint>
#include <netinet/in.h>

namespace mzx
{

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
