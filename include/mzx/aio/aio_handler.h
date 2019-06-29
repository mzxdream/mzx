#ifndef __MZX_AIO_HANDLER_H__
#define __MZX_AIO_HANDLER_H__

#include <functional>

#include <mzx/error.h>

namespace mzx
{

class AIOHandler final
{
public:
    using ReadCallback = std::function<void()>;
    using WriteCallback = std::function<void()>;
    using CloseCallback = std::function<void(const Error &)>;

    explicit AIOHandler(int fd, int efd);
    ~AIOHandler();
    AIOHandler(const AIOHandler &) = delete;
    AIOHandler &operator=(const AIOHandler &) = delete;

public:
    void SetReadCallback(ReadCallback cb);
    void SetWriteCallback(WriteCallback cb);
    void SetCloseCallback(CloseCallback cb);

    void EnableRead(bool enable = true);
    void EnableWrite(bool enable = true);
    void EnableAll(bool enable = true);

private:
    int fd_{-1};
    int efd_{-1};
    int events_{0};
    ReadCallback read_cb_;
    WriteCallback write_cb_;
    CloseCallback close_cb_;
};

} // namespace mzx

#endif
