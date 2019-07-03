#ifndef __MZX_TCP_CONNECTOR_H__
#define __MZX_TCP_CONNECTOR_H__

#include <functional>
#include <list>

#include <mzx/aio/aio_handler.h>
#include <mzx/aio/aio_server.h>
#include <mzx/aio/net/net_define.h>
#include <mzx/aio/net/tcp_socket.h>
#include <mzx/error.h>

namespace mzx
{

class TcpAcceptor;

class TcpConnector final
{
    friend TcpAcceptor;

public:
    using ConnectCallback = std::function<void(const Error &)>;
    using ReadCallback = std::function<void(const Error &)>;
    using WriteCallback = std::function<void(const Error &)>;

    struct ConnectInfo
    {
        ConnectInfo() = default;
        ConnectInfo(const NetAddress &a, ConnectCallback cb)
            : address(a)
            , callback(cb)
        {
        }
        NetAddress address;
        ConnectCallback callback;
    };

    struct ReadInfo
    {
        ReadInfo() = default;
        ReadInfo(char *d, std::size_t s, std::size_t r, ReadCallback cb)
            : data(d)
            , size(s)
            , read_size(r)
            , callback(cb)
        {
        }
        char *data{nullptr};
        std::size_t size{0};
        std::size_t read_size{0};
        ReadCallback callback;
    };

    struct WriteInfo
    {
        WriteInfo() = default;
        WriteInfo(const char *d, std::size_t s, std::size_t w, WriteCallback cb)
            : data(d)
            , size(s)
            , write_size(w)
            , callback(cb)
        {
        }
        const char *data{nullptr};
        std::size_t size{0};
        std::size_t write_size{0};
        WriteCallback callback;
    };

    explicit TcpConnector(AIOServer &aio_server);
    explicit TcpConnector(AIOServer &aio_server, const NetAddress &addr,
                          bool reuse_addr = true);
    ~TcpConnector();
    TcpConnector(const TcpConnector &) = delete;
    TcpConnector &operator=(const TcpConnector &) = delete;

public:
    AIOServer &GetAIOServer();
    bool Open(bool is_ipv6 = false);
    bool Bind(const NetAddress &addr);
    bool SetReuseAddr();
    void Close();

    void AsyncConnect(const NetAddress &addr, ConnectCallback cb,
                      bool forcePost = false);
    void AsyncRead(char *data, std::size_t size, ReadCallback cb,
                   bool forcePost = false);
    void AsyncWrite(const char *data, std::size_t size, WriteCallback cb,
                    bool forcePost = false);

private:
    void OnAddConnect(const NetAddress &addr, ConnectCallback cb);
    void OnAddRead(char *data, std::size_t size, ReadCallback cb);
    void OnAddWrite(const char *data, std::size_t size, WriteCallback cb);
    void OnConnect();
    void OnRead();
    void OnWrite();
    void OnClose(const Error &error);

private:
    AIOServer &aio_server_;
    TcpSocket sock_;
    bool is_connected_{false};
    AIOHandler aio_handler_;
    std::list<ConnectInfo> connect_list_;
    std::list<ReadInfo> read_list_;
    std::list<WriteInfo> write_list_;
};

} // namespace mzx

#endif
