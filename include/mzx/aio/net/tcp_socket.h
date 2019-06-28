#ifndef __MZX_TCP_SOCKET_H__
#define __MZX_TCP_SOCKET_H__

#include <mzx/aio/aio_server.h>
#include <mzx/aio/net/net_define.h>

namespace mzx
{

class TcpSocket final
{
public:
    TcpSocket(AIOServer &aio_server);
    TcpSocket(AIOServer &aio_server, bool is_ipv6);
    TcpSocket(AIOServer &aio_server, const NetAddress &addr);
    ~TcpSocket();
    TcpSocket(const TcpSocket &) = delete;
    TcpSocket &operator=(const TcpSocket &) = delete;

public:
    AIOServer &GetAIOServer();
    bool IsOpen() const;

    bool Open(bool is_ipv6);
    void Close();
    bool Bind(const NetAddress &addr);
    bool SetReuseAddr();
    bool Listen(int backlog);
    bool Accept(TcpSocket *sock, NetAddress *addr = nullptr);

private:
    AIOServer &aio_server_;
    int sock_{-1};
};

} // namespace mzx

#endif
