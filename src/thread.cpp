#include <mzx/logger.h>
#include <mzx/thread.h>

namespace mzx
{

Thread::Thread()
{
}

Thread::Thread(std::function<void()> run_cb)
    : run_cb_(run_cb)
{
    MZX_CHECK(run_cb != nullptr);
}

Thread::~Thread()
{
    MZX_CHECK(joinable_);
}

ThreadID Thread::ID() const
{
    return thread_id_;
}

bool Thread::Joinable() const
{
    return joinable_;
}

bool Thread::Start()
{
    if (Joinable())
    {
        MZX_WARN("thread:", thread_id_, " is running");
        return false;
    }
    auto err = pthread_create(&thread_id_, nullptr, &Thread::Run, this);
    if (err != 0)
    {
        MZX_WARN("create thread failed:", err);
        return false;
    }
    return true;
}

bool Thread::Join()
{
    if (!Joinable())
    {
        return true;
    }
    int err = pthread_join(thread_id_, nullptr);
    if (err != 0)
    {
        MZX_WARN("thread:", thread_id_, " join failed:", err);
        return false;
    }
    return true;
}

bool Thread::Cancel()
{
    return Cancel(thread_id_);
}

ThreadID Thread::CurrentID()
{
    return pthread_self();
}

void Thread::CheckCancelPoint()
{
    pthread_testcancel();
}

bool Thread::Cancel(ThreadID id)
{
    int err = pthread_cancel(id);
    if (err != 0)
    {
        MZX_WARN("thread:", id, " cancel failed:", err);
        return false;
    }
    return true;
}

void *Thread::Run(void *param)
{
    auto *th = static_cast<Thread *>(param);
    MZX_CHECK(th != nullptr);
    pthread_cleanup_push(&Thread::CleanUp, param);
    _Run();
    if (run_cb_)
    {
        run_cb_();
    }
    pthread_cleanup_pop(1);
}

void Thread::CleanUp(void *param)
{
    auto *th = static_cast<Thread *>(param);
    MZX_CHECK(th != nullptr);
    th->joinable_ = false;
}

} // namespace mzx
