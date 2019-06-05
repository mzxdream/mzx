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
}

ThreadID Thread::ID() const
{
    return thread_id_;
}

bool Thread::Joinable() const
{
    return thread_id_ != THREAD_ID_INVALID;
}

static void *ThreadMain(void *param)
{
    MZX_CHECK(param != nullptr);
    static_cast<mzx::Thread *>(param)->Run();
    return nullptr;
}

bool Thread::Start()
{
    if (Joinable())
    {
        MZX_WARN("thread:", thread_id_, " is running");
        return false;
    }
    auto err = pthread_create(&thread_id_, nullptr, &ThreadMain, this);
    if (err != 0)
    {
        MZX_WARN("create thread failed:", err);
        return false;
    }
    return true;
}

bool Thread::Join()
{
    if (thread_id_ == THREAD_ID_INVALID)
    {
        return true;
    }
    int err = pthread_join(thread_id_, nullptr);
    if (err != 0)
    {
        MZX_WARN("thread:", thread_id_, " join failed:", err);
        return false;
    }
    thread_id_ = THREAD_ID_INVALID;
    return true;
}

bool Thread::Cancel()
{
    return Cancel(thread_id_);
}

void Thread::CheckCancelPoint()
{
    pthread_testcancel();
}

void Thread::Run()
{
    _Run();
    if (run_cb_)
    {
        run_cb_();
    }
}

ThreadID Thread::CurrentID()
{
    return pthread_self();
}

bool Thread::Cancel(ThreadID id)
{
    if (id == THREAD_ID_INVALID)
    {
        return true;
    }
    int err = pthread_cancel(id);
    if (err != 0)
    {
        MZX_WARN("thread:", id, " cancel failed:", err);
        return false;
    }
    return true;
}

} // namespace mzx
