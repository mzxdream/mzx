#ifndef __MZX_TCP_SOCKET_H__
#define __MZX_TCP_SOCKET_H__

#include <mzx/aio/aio_server.h>
#include <mzx/aio/net/net_define.h>

namespace mzx
{

class TcpSocket final
{
public:
    TcpSocket();
    TcpSocket(bool is_ipv6);
    TcpSocket(const NetAddress &addr, bool reuse_addr = true);
    ~TcpSocket();
    TcpSocket(const TcpSocket &) = delete;
    TcpSocket &operator=(const TcpSocket &) = delete;

public:
    int GetSock() const;
    bool IsOpen() const;

    bool Open(bool is_ipv6);
    void Close();
    bool Bind(const NetAddress &addr);
    bool SetReuseAddr();
    bool Listen(int backlog);
    bool Accept(TcpSocket *sock, NetAddress *addr = nullptr);

private:
    int sock_{-1};
};

} // namespace mzx

#endif
