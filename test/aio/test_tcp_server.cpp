#include <iostream>

#include <mzx/aio/aio_server.h>
#include <mzx/aio/net/tcp_acceptor.h>
#include <mzx/aio/net/tcp_connector.h>

TcpStream *tcp_stream = nullptr;
char stream_buf[1024] = {0};
std::size_t buf_len = 0;

static void OnRead(mzx::TcpStream *stream, const mzx::Error &error)
{
    if (error)
    {
        std::cout << "error:" << error.errorno << std::endl;
        return;
    }
    std::size_t read_len = 0;
    auto e = stream->Read(stream_buf + buf_len, sizeof(stream_buf) - buf_len,
                          &read_len);
    if (e)
    {
        std::cout << "error:" << e.errorno << std::endl;
        return;
    }
    buf_len += read_len;
    if (buf_len > 100)
    {
        stream_buf[buf_len] = '\0';
        std::cout << stream_buf << std::endl;
        buf_len = 0;
    }
    // TODO
}

static void OnAccept(mzx::TcpAcceptor *acceptor, const mzx::Error &error)
{
    if (error)
    {
        std::cout << "error:" << error.errorno << std::endl;
        return;
    }

    tcp_stream = new TcpStream(acceptor->GetAIOServer());
    if (!acceptor->Accept(tcp_stream))
    {
        std::cout << "accept failed" << std::endl;
        return;
    }
    tcp_stream->StartRead();
    // TODO
}

int main(int argc, char *argv[])
{
    mzx::AIOServer aio_server;
    mzx::TcpAcceptor acceptor(aio_server);
    acceptor.Bind(NetAddress(8282));
    acceptor.SetReuseAddr();
    acceptor.StartAccept();

    io_server.Start();
    io_server.Join();
    return 0;
}
