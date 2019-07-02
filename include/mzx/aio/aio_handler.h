#ifndef __MZX_AIO_HANDLER_H__
#define __MZX_AIO_HANDLER_H__

#include <functional>

#include <mzx/error.h>

namespace mzx
{

class AIOServer;

class AIOHandler final
{
    friend AIOServer;

public:
    using ReadCallback = std::function<void()>;
    using WriteCallback = std::function<void()>;
    using CloseCallback = std::function<void(const Error &)>;

    explicit AIOHandler(AIOServer &aio_server);
    explicit AIOHandler(AIOServer &aio_server, int fd);
    ~AIOHandler();
    AIOHandler(const AIOHandler &) = delete;
    AIOHandler &operator=(const AIOHandler &) = delete;

public:
    void SetFD(int fd);
    void SetReadCallback(ReadCallback cb);
    void SetWriteCallback(WriteCallback cb);
    void SetCloseCallback(CloseCallback cb);

    void EnableRead(bool enable = true);
    void EnableWrite(bool enable = true);
    void EnableAll(bool enable = true);

private:
    void HandleEvent(int events);

private:
    AIOServer &aio_server_;
    int fd_{-1};
    int events_{0};
    ReadCallback read_cb_;
    WriteCallback write_cb_;
    CloseCallback close_cb_;
};

} // namespace mzx

#endif
