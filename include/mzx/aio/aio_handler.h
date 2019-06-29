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

    AIOHandler();
    ~AIOHandler();
    AIOHandler(const AIOHandler &) = delete;
    AIOHandler &operator=(const AIOHandler &) = delete;

private:
    int fd_{-1};
    ReadCallback read_cb_;
    WriteCallback write_cb_;
    std::function<void(const Error &)> close_cb_;
};

} // namespace mzx

#endif
