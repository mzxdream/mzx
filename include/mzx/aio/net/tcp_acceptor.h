#ifndef __MZX_TCP_ACCEPTOR_H__
#define __MZX_TCP_ACCEPTOR_H__

#include <functional>

#include <mzx/aio/aio_server.h>
#include <mzx/aio/net/net_define.h>
#include <mzx/aio/net/tcp_socket.h>
#include <mzx/error.h>

namespace mzx
{

class TcpAcceptor final
{
public:
    using AcceptCallback = std::function<void(const Error &)>;

    explicit TcpAcceptor(AIOServer &aio_server);
    explicit TcpAcceptor(AIOServer &aio_server, const NetAddress &addr,
                         bool reuse_addr = true);
    ~TcpAcceptor();
    TcpAcceptor(const TcpAcceptor &) = delete;
    TcpAcceptor &operator=(const TcpAcceptor &) = delete;

public:
    AIOServer &GetAIOServer();
    bool Open(bool is_ipv6 = false);
    bool Bind(const NetAddress &addr);
    bool SetReuseAddr();
    bool Listen(int backlog = 128);

    void AsyncAccept(TcpSocket *socket, AcceptCallback cb,
                     bool forcePost = false);

private:
    TcpSocket sock_;
};

}; // namespace mzx

#endif
