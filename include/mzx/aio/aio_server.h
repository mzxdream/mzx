#ifndef __MZX_AIO_SERVER_H__
#define __MZX_AIO_SERVER_H__

#include <functional>
#include <list>
#include <mutex>

#include <mzx/thread.h>

namespace mzx
{

class AIOServer final : public Thread
{
public:
    using ExecFunc = std::function<void()>;

    explicit AIOServer();
    virtual ~AIOServer();
    AIOServer(const AIOServer &) = delete;
    AIOServer &operator=(const AIOServer &) = delete;

    void Exec(ExecFunc func);
    void Post(ExecFunc func);

private:
    virtual void _Run() override;

private:
    std::mutex exec_queue_mtx;
    std::list<ExecFunc> exec_queue_;
};

} // namespace mzx

#endif
