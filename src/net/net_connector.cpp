#include <cstring>

#include <mzx/logger.h>
#include <mzx/net/net_connector.h>
#include <mzx/net/net_worker.h>

namespace mzx
{

NetConnector::NetConnector(NetWorker &net_worker)
    : net_worker_(net_worker)
    , sock_(net_worker.GetIOService())
{
}

NetConnector::~NetConnector()
{
}

asio::ip::tcp::socket &NetConnector::GetSock()
{
    return sock_;
}

void NetConnector::SetReadCallback(ReadCallback cb)
{
    read_cb_ = cb;
}

void NetConnector::SetCloseCallback(CloseCallback cb)
{
    close_cb_ = cb;
}

void NetConnector::Start()
{
    ReadLength();
}

void NetConnector::Close()
{
    sock_.close();
}

void NetConnector::Write(const char *data, std::size_t size)
{
    MZX_CHECK(data != nullptr && size > 0 && size <= NET_MSG_MAX_LENGTH);
    auto *msg = new char[size + sizeof(NetMsgLenType)];
    auto msg_length = ToNetMsgLen(static_cast<NetMsgLenType>(size));
    memcpy(msg, &msg_length, sizeof(NetMsgLenType));
    memcpy(msg + sizeof(NetMsgLenType), data, size);
    asio::async_write(sock_, asio::buffer(msg, msg_length),
                      [this, msg, msg_length](std::error_code ec, std::size_t) {
                          if (ec)
                          {
                              net_worker_.PostOut([this, ec]() {
                                  if (close_cb_)
                                  {
                                      close_cb_(ec);
                                  }
                              });
                          }
                          delete msg;
                      });
}

void NetConnector::ReadLength()
{
    asio::async_read(sock_, asio::buffer(&msg_length_, sizeof(msg_length_)),
                     [this](std::error_code ec, std::size_t) {
                         if (!ec)
                         {
                             ReadContent();
                         }
                         else
                         {
                             net_worker_.PostOut([this, ec]() {
                                 if (close_cb_)
                                 {
                                     close_cb_(ec);
                                 }
                             });
                         }
                     });
}

void NetConnector::ReadContent()
{
    auto msg_length = ToHostMsgLen(msg_length_);
    if (msg_length > NET_MSG_MAX_LENGTH)
    {
        // TODO net_worker_.PostOut([]);
        return;
    }
    char *msg = new char[msg_length];
    asio::async_read(sock_, asio::buffer(msg, msg_length),
                     [this](std::error_code ec, std::size_t) {
                         if (!ec)
                         {
                             net_worker_.PostOut([this, msg, msg_length]() {
                                 if (read_cb_)
                                 {
                                     read_cb_(msg, msg_length);
                                 }
                                 delete msg;
                             });
                             ReadLength();
                         }
                         else
                         {

                             net_worker_.PostOut([this, ec]() {
                                 if (close_cb_)
                                 {
                                     close_cb_(ec);
                                 }
                             });
                         }
                     });
}

} // namespace mzx
