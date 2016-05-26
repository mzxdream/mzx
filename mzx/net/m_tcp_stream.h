#ifndef _M_TCP_STREAM_H_
#define _M_TCP_STREAM_H_

#include <mzx/util/m_type_define.h>
#include <mzx/util/m_errno.h>
#include <mzx/event/m_event_loop.h>
#include <functional>
#include <list>

struct MTcpReadCache
{
    char *p_buf;
    std::size_t len;
    std::size_t pos_start;
    std::size_t pos_end;
};

struct MTcpReadBuffer
{
    char *p_buf;
    std::size_t min_len;
    std::string max_len;
    std::size_t read_len;
};

struct MTcpWriteBuffer
{
    const char *p_buf;
    std::size_t len;
    std::size_t write_len;
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
    MError Init(MEventLoop *p_event_loop, int fd, std::size_t read_cache_len);
    void Clear();

    MError EnableRead(bool enable);
    MError EnableWrite(bool enable);

    MError AsyncRead(char *p_buf, std::size_t min_len, std::size_t max_len, const std::function<void (std::size_t, MError)> &read_cb);
    MError AsyncWrite(const char *p_buf, std::size_t len, const std::function<void (MError)> &write_cb);
private:
    virtual void _OnCallback(unsigned events) override;
private:
    int fd_;
    MTcpReadCache read_cache_;
    bool read_enable_;
    bool write_enable_;
    bool readable_;
    bool writable_;
    std::list<MTcpReadBuffer> read_buffers_;
    std::list<MTcpWriteBuffer> write_buffers_;
};

#endif
