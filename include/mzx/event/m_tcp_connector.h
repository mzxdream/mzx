#ifndef _M_TCP_CONNECTOR_H_
#define _M_TCP_CONNECTOR_H_

#include <mzx/util/m_errno.h>
#include <mzx/event/m_event_loop.h>
#include <functional>
#include <list>

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
    const char *p_buf;
    std::size_t len;
    std::size_t min_len;
    std::size_t write_len;
    std::function<void (std::size_t, MError)> write_cb;
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
    MError StopConnect();

    MError AsyncRead(char *p_buf, std::size_t len, const std::function<void (std::size_t, MError)> &read_cb);
    MError AsyncReadSome(char *p_buf, std::size_t len, std::size_t min_len, const std::function<void (std::size_t, MError)> &read_cb);
    MError StopRead();

    MError AsyncWrite(const char *p_buf, std::size_t len, const std::function<void (std::size_t, MError)> &write_cb);
    MError AsyncWriteSome(const char *p_buf, std::size_t len, std::size_t min_len, const std::function<void (std::size_t, MError)> &write_cb);
    MError StopWrite();
private:
    void OnError(MError err);
public:
    void OnConnectCallback(unsigned events);
    void OnStreamCallback(unsigned events);
private:
    MEventLoop *p_event_loop_;
    int fd_;
    bool close_on_free_;
    bool readable_;
    bool writable_;
    std::function<void (MError)> connect_cb_;
    std::list<MTcpReadBuffer> read_buffers_;
    std::list<MTcpWriteBuffer> write_buffers_;
};

#endif