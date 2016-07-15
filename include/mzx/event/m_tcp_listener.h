#ifndef _M_TCP_LISTENER_H_
#define _M_TCP_LISTENER_H_

#include <mzx/util/m_errno.h>
#include <mzx/event/m_event_loop.h>
#include <functional>
#include <string>
#include <list>

class MTcpListener
{
public:
    MTcpListener();
    ~MTcpListener();
    MTcpListener(const MTcpListener &) = delete;
    MTcpListener& operator=(const MTcpListener &) = delete;
public:
    MError Init(MEventLoop *p_event_loop, int fd, bool close_on_free);
    void Clear();

    MError AsyncAccept(const std::function<void (int, std::string, unsigned, MError)> &cb);
    MError StopAccept();
public:
    void OnError(MError err);
    void OnAcceptCallback(unsigned events);
private:
    MEventLoop *p_event_loop_;
    int fd_;
    bool close_on_free_;
    bool acceptable_;
    std::list<std::function<void (int, std::string, unsigned, MError)> > accept_buffers_;
};

#endif
