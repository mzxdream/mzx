#ifndef _M_TCP_CONNECTOR_H_
#define _M_TCP_CONNECTOR_H_

#include <mzx/util/m_errno.h>
#include <mzx/event/m_event_loop.h>
#include <functional>

struct MTcpReadBuffer
{
    char *p_buf;
    std::size_t len;
    std::size_t min_len;
    std::size_t read_len;
    std::function<void (std::size_t, MError)> read_cb;
};

struct MTcpWriteBuffer
{
    char *p_buf;
    std::size_t len;
    std::size_t min_len;
    std::size_t write_len;
    std::function<void (MError)> write_cb;
};

class MTcpConnector
{
public:
    MTcpConnector();
    ~MTcpConnector();
    MTcpConnector(const MTcpConnector &) = delete;
    MTcpConnector& operator=(const MTcpConnector &) = delete;
public:
    MError Init(MEventLoop *p_event_loop, int fd, bool close_on_free);
    void Clear();

    MError AsyncConnect(const std::string &ip, unsigned port, const std::function<void (MError)> &cb);
    MError AsyncRead(char *p_buf, std::size_t len, std::size_t min_len, const std::function<void (std::size_t, MError)> &read_cb);
    MError AysncWrite(const char *p_buf, std::size_t len, std::size_t min_len, const std::function<void (MError)> &write_cb);

    MError ClearRead();
    MError ClearWrite();
    MError ClearReadWrite();
private:
    void OnError(MError err);
public:
    void OnConnectCallback(unsigned events);
    void OnStreamCallback(unsigned events);
private:
    MIOEventBase event_base_;
    bool close_on_free_;
    bool readable_;
    bool writable_;
    std::function<void (MError)> connect_cb_;
    std::list<MTcpReadBuffer> read_buffers_;
    std::list<MTcpWriteBuffer> write_buffers_;
};

#endif
