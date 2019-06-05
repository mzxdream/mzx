#ifndef __MZX_THREAD_H__
#define __MZX_THREAD_H__

#include <functional>
#include <pthread.h>
#include <string>

namespace mzx
{

using ThreadID = pthread_t;
constexpr auto THREAD_ID_INVALID = static_cast<ThreadID>(0);

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

public:
    static ThreadID CurrentID();
    static void CheckCancelPoint();
    static bool Cancel(ThreadID id);

private:
    static void *Run(void *param);
    static void CleanUp(void *param);

private:
    virtual void _Run()
    {
    }

private:
    ThreadID thread_id_{THREAD_ID_INVALID};
    volatile bool joinable_{false};
    std::function<void()> run_cb_;
};

} // namespace mzx

#endif
