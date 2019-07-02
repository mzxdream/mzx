#ifndef __MZX_TCP_SOCKET_H__
#define __MZX_TCP_SOCKET_H__

#include <mzx/aio/net/net_define.h>
#include <mzx/error.h>

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

    Error Accept(TcpSocket *sock, NetAddress *addr = nullptr);
    Error Connect(const NetAddress &addr);
    Error Read(char *data, std::size_t size, std::size_t *read_size = nullptr);
    Error Write(const char *data, std::size_t size,
                std::size_t *write_size = nullptr);

private:
    int sock_{-1};
};

} // namespace mzx

#endif
