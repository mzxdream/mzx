#ifndef __MZX_AIO_HANDLER_H__
#define __MZX_AIO_HANDLER_H__

#include <functional>

#include <mzx/error.h>

namespace mzx
{

class AIOServer;

struct AIOOperation
{
public:
    AIOOperation() = default;
    virtual ~AIOOperation() = 0;

    virtual bool Perform()
    {
        return true;
    }
    virtual void Complete()
    {
    }
};

class AIOHandler final
{
    friend AIOServer;

public:
    using ReadCallback = std::function<void(const Error &)>;
    using WriteCallback = std::function<void(const Error &)>;

    explicit AIOHandler(AIOServer &aio_server);
    explicit AIOHandler(AIOServer &aio_server, int fd);
    ~AIOHandler();
    AIOHandler(const AIOHandler &) = delete;
    AIOHandler &operator=(const AIOHandler &) = delete;

public:
    bool Assign(int fd);
    void Close();
    void StartRead(ReadCallback cb);
    void StartWrite(WriteCallback cb);
    void StopRead();
    void StopWrite();

private:
    void HandleEvent(int events);

private:
    AIOServer &aio_server_;
    int fd_{-1};
    int events_{0};
    ReadCallback read_cb_;
    WriteCallback write_cb_;
};

} // namespace mzx

;
