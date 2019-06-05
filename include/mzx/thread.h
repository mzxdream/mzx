#ifndef __MZX_THREAD_H__
#define __MZX_THREAD_H__

#include <functional>
#include <pthread.h>
#include <string>

namespace mzx
{

using ThreadID = pthread_t;
constexpr THREAD_ID_INVALID = static_cast<ThreadID>(0);

class Thread
{
protected:
    Thread();

public:
    explicit Thread(std::function<void()> run_cb);
    virtual ~Thread();
    Thread(const Thread &) = delete;
    Thread &operator=(const Thread &) = delete;

public:
    ThreadID ID() const;
    bool Joinable() const;

    bool Start();
    bool Join();
    bool Cancel();
    void CheckCancelPoint();
    void Run();

public:
    static ThreadID CurrentID();
    static bool Cancel(ThreadID id);

private:
    virtual void _Run()
    {
    }

private:
    volatile ThreadID thread_id_{THREAD_ID_INVALID};
    std::function<void()> run_cb_;
};

} // namespace mzx

#endif
