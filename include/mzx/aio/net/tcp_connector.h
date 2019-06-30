#ifndef __MZX_TCP_CONNECTOR_H__
#define __MZX_TCP_CONNECTOR_H__

#include <functional>
#include <list>

#include <mzx/aio/aio_handler.h>
#include <mzx/aio/aio_server.h>
#include <mzx/aio/net/net_define.h>
#include <mzx/aio/net/tcp_socket.h>
#include <mzx/error.h>

namespace mzx
{

class TcpConnector final
{
public:
    using ReadCallback = std::function<void(const Error &)>;

    explicit TcpConnector();
private:
};

} // namespace mzx

#endif
