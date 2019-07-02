#ifndef __MZX_TCP_ACCEPTOR_H__
#define __MZX_TCP_ACCEPTOR_H__

#include <functional>
#include <list>

#include <mzx/aio/aio_handler.h>
#include <mzx/aio/aio_server.h>
#include <mzx/aio/net/net_define.h>
#include <mzx/aio/net/tcp_connector.h>
#include <mzx/aio/net/tcp_socket.h>
#include <mzx/error.h>

namespace mzx
{

class TcpAcceptor final
{
public:
    using AcceptCallback = std::function<void(const Error &)>;
    struct AcceptInfo
    {
        AcceptInfo() = default;
        AcceptInfo(TcpConnector *conn, AcceptCallback cb)
            : connector(conn)
            , callback(cb)
        {
        }
        TcpConnector *connector{nullptr};
        AcceptCallback callback;
    };

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

    void AsyncAccept(TcpConnector *conn, AcceptCallback cb,
                     bool forcePost = false);

private:
    void OnAddAccept(TcpConnector *conn, AcceptCallback cb);
    void OnAccept();
    void OnClose(const Error &error);

private:
    AIOServer &aio_server_;
    TcpSocket sock_;
    AIOHandler aio_handler_;
    std::list<AcceptInfo> accept_list_;
};

}; // namespace mzx

#endif
