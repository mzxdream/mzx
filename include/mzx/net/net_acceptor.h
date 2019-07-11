#ifndef __MZX_NET_ACCEPTOR_H__
#define __MZX_NET_ACCEPTOR_H__

#include <asio.hpp>
#include <functional>

namespace mzx
{

class NetWorker;

class NetAccpetor final
{
public:
    using 
public:
    NetAccpetor(NetWorker &net_worker, const std::string &ip,
                unsigned short port);
    ~NetAccpetor();
    NetAccpetor(const NetAccpetor &) = delete;
    NetAccpetor &operator=(const NetAccpetor &) = delete;

public:
    void Start();

private:
    NetWorker net_worker_;
    asio::ip::tcp::acceptor acceptor_;
    std::function<void()>
};

} // namespace mzx

#endif
