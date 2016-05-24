#ifndef _M_TCP_EVENT_H_
#define _M_TCP_EVENT_H_

#include <mzx/event/m_event_base.h>
#include <functional>

class MTcpEvent
    :public MIOEventBase
{
public:
    MTcpEvent();
    virtual ~MTcpEvent();
    MTcpEvent(const MTcpEvent &) = delete;
    MTcpEvent& operator=(const MTcpEvent &) = delete;
public:
    MError Init(MEventLoop *p_event_loop, int fd);
    void Clear();

    MError StartConnect(const std::string &ip, unsigned port, const std::function<void (MError)> &cb);
    MError StopConnect();

    MError StartListen(int backlog, const std::function<void (MError)> &cb);
    MError StopListen();

    MError StartReadSome(void *p_buf, int len, const std::function<void (int, MError)> &cb);
    MError StopReadSome();

    MError StartRead(void *p_buf, int len, const std::function<void (MError)> &cb);
    MError StopRead();

    MError StartWriteSome(const char *p_buf, int len, const std::function<void (int, MError)> &cb);
    MError StopWriteSome();

    MError StartWrite(const char *p_buf, int len, const std::function<void (MError)> &cb);
    MError StopWrite();
public:
    void OnConnectCallback(unsigned events);
    void OnListenCallback(unsigned events);
    void OnReadSomeCallback(unsigned events);
    void OnReadCallback(unsigned events);
    void OnWriteSomeCallback(unsigned events);
    void OnWriteCallback(unsigned events);
private:
    virtual void _OnCallback(unsigned events) override;
private:
    std::function<void (unsigned)> cb_;
    std::function<void (MError)> connect_cb_;
    std::function<void (MError)> listen_cb_;
    std::function<void (int, MError)> read_some_cb_;
    std::function<void (MError)> read_cb_;
    std::function<void (int, MError)> write_some_cb_;
    std::function<void (MError)> write_cb_;
};

#endif
