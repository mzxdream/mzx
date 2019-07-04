#include <iostream>

#include <mzx/aio/aio_server.h>
#include <mzx/aio/net/tcp_acceptor.h>
#include <mzx/aio/net/tcp_connector.h>

void OnWrite(mzx::TcpConnector *conn, char *data, const mzx::Error &error);

void OnRead(mzx::TcpConnector *conn, char *data, const mzx::Error &error)
{
    if (error)
    {
        delete[] data;
        std::cout << "error:" << (int)error.Type() << std::endl;
        delete conn;
        return;
    }
    data[10] = '\0';
    std::cout << "read success:" << data << std::endl;
    delete[] data;
    mzx::Thread::Sleep(5000);
    data = new char[10];
    for (int i = 0; i < 10; ++i)
    {
        data[i] = 'B' + 1;
    }
    conn->AsyncWrite(data, 10,
                     std::bind(OnWrite, conn, data, std::placeholders::_1));
}

void OnWrite(mzx::TcpConnector *conn, char *data, const mzx::Error &error)
{
    delete[] data;
    if (error)
    {
        std::cout << "error:" << (int)error.Type() << std::endl;
        delete conn;
        return;
    }
    std::cout << "write success" << std::endl;
    data = new char[11];
    conn->AsyncRead(data, 10,
                    std::bind(OnRead, conn, data, std::placeholders::_1));
}

void OnAccept(mzx::TcpAcceptor *acceptor, mzx::TcpConnector *conn,
              const mzx::Error &error)
{
    if (error)
    {
        delete conn;
        std::cout << "error:" << (int)error.Type() << std::endl;
        return;
    }
    std::cout << "accept success" << std::endl;
    char *data = new char[11];
    conn->AsyncRead(data, 10,
                    std::bind(OnRead, conn, data, std::placeholders::_1));
    acceptor->AsyncAccept(
        conn, std::bind(OnAccept, acceptor, conn, std::placeholders::_1));
}

int main(int argc, char *argv[])
{
    mzx::AIOServer aio_server;
    mzx::TcpAcceptor acceptor(aio_server, mzx::NetAddress(8082));
    auto *conn = new mzx::TcpConnector(aio_server);
    acceptor.AsyncAccept(
        conn, std::bind(OnAccept, &acceptor, conn, std::placeholders::_1));
    aio_server.Start();
    aio_server.Join();
    return 0;
}
