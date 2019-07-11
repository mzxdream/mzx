#ifndef __MZX_NET_CONNECTOR_H__
#define __MZX_NET_CONNECTOR_H__

#include <asio.hpp>
#include <cstdint>
#include <memory>

#include <mzx/error.h>
#include <mzx/net/net_define.h>

namespace mzx
{

class NetWorker;

class NetConnector final
{
public:
    using ReadCallback = std::function<void(const char *, std::size_t)>;
    using CloseCallback = std::function<void(std::error_code)>;

public:
    explicit NetConnector(NetWorker &net_worker);
    ~NetConnector();
    NetConnector(const NetConnector &) = delete;
    NetConnector &operator=(const NetConnector &) = delete;

public:
    asio::ip::tcp::socket &GetSock();

    void SetReadCallback(ReadCallback cb);
    void SetCloseCallback(CloseCallback cb);

    void Start();
    void Close();
    void Write(const char *data, std::size_t size);

private:
    void ReadLength();
    void ReadContent();

private:
    NetWorker &net_worker_;
    asio::ip::tcp::socket sock_;
    ReadCallback read_cb_;
    CloseCallback close_cb_;
    NetMessageLengthType msg_length_{0};
};

} // namespace mzx

#endif
