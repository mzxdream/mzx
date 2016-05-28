#ifndef _M_TCP_STREAM_H_
#define _M_TCP_STREAM_H_

#include <mzx/util/m_type_define.h>
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
    std::size_t write_len;
    std::function<void (MError)> write_cb;
};

class MTcpStream
    :public MIOEventBase
{
public:
    MTcpStream();
    virtual ~MTcpStream();
    MTcpStream(const MTcpStream &) = delete;
    MTcpStream& operator=(const MTcpStream &) = delete;
public:
    MError Init(MEventLoop *p_event_loop, int fd);
    void Clear();

    MError Read(char *p_buf, std::size_t len, std::size_t min_len, const std::function<void (std::size_t, MError)> &read_cb);
    void StopRead();

    MError Write(const char *p_buf, std::size_t len, const std::function<void (MError)> &write_cb);
    void StopWrite();
private:
    void OnError(MError err);
    virtual void _OnCallback(unsigned events) override;
private:
    int fd_;
    bool readable_;
    bool writable_;
    std::list<MTcpReadBuffer> read_buffers_;
    std::list<MTcpWriteBuffer> write_buffers_;
};

#endif
