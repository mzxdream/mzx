#ifndef _M_TCP_CONNECTOR_H_
#define _M_TCP_CONNECTOR_H_

#include <mzx/util/m_errno.h>
#include <mzx/event/m_event_loop.h>
#include <functional>

class MTcpReadBuffer
{
public:
    MTcpReadBuffer();
    ~MTcpReadBuffer();
    MTcpReadBuffer(const MTcpReadBuffer &) = delete;
    MTcpReadBuffer& operator=(const MTcpReadBuffer &) = delete;
public:
    std::size_t GetReadLen() const;

    MError Init(std::size_t len);
    void Clear();

    MError ReadFD(int fd);

    MError Read(const char *p_buf, std::size_t len);
private:
    char *p_buf_;
    std::size_t len_;
    char *p_start_;
    char *p_end_;
};


class MTcpWriteBuffer
{
public:
    MTcpWriteBuffer();
    ~MTcpWriteBuffer();
    MTcpWriteBuffer(const MTcpWriteBuffer &) = delete;
    MTcpWriteBuffer& operator=(const MTcpWriteBuffer &) = delete;
public:
    MError Init(const char *p_buf, std::size_t len);
    void Clear();

    MError WriteFD(int fd);
private:
    char *p_buf_;
    std::size_t len_;
    std::size_t write_len_;
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

    MError StartConnect(const std::string &ip, unsigned port, const std::function<void (MError)> &cb);
    MError StopConnect();

    MError StartRead(std::size_t max_len, const std::function<void (MError)> &cb);
    MError StopRead();

    MError Write(const char *p_buf, std::size_t len, const std::function<void (MError)> &write_cb);
    MError StopWrite();
public:
    void OnConnectCallback(unsigned events);
    void OnStreamCallback(unsigned events);
private:
    MIOEventBase event_base_;
    bool close_on_free_;
    bool readable_;
    bool writable_;
    std::function<void (MError)> connect_cb_;
    std::function<void (MError)> read_cb_;
    MTcpReadBuffer read_buffer_;
    std::list<MTcpWriteBuffer> write_buffers_;
};

#endif
