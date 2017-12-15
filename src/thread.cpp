#include <mzx/thread.h>
#include <mzx/logger.h>

namespace mzx {

Thread::Thread()
    : pid_(PID_INVALID)
    , stop_flag_(false)
{
}

Thread::~Thread()
{
    StopAndJoin();
}

bool Thread::Start()
{
    if (pid_ != PID_INVALID)
    {
        MWARN("thread is running");
        return false;
    }
    stop_flag_ = false;
    int err = pthread_create(&pid_, nullptr, &ThreadMain, this);
    if (err != 0)
    {
        MWARN("create thread failed:", err);
        return false;
    }
    return true;
}

void Thread::Stop()
{
    stop_flag_ = true;
}

bool Thread::Join()
{
    if (pid_ == PID_INVALID)
    {
        return true;
    }
    int err = pthread_join(pid_, nullptr);
    if (err != 0)
    {
        MWARN("thread join failed:", err);
        return false;
    }
    pid_ = PID_INVALID;
    return true;
}

bool Thread::StopAndJoin()
{
    Stop();
    return Join();
}

Thread::PID Thread::GetPID() const
{
    return pid_;
}

Thread::PID Thread::GetCurPID()
{
    return pthread_self();
}

void* Thread::ThreadMain(void *param)
{
    Thread *th = static_cast<Thread*>(param);
    if (!th)
    {
        MERR("param is null");
        return nullptr;
    }
    if (!th->_BeforeRun())
    {
        MERR("before run failed");
        return nullptr;
    }
    while (!th->stop_flag_)
    {
        th->_Run();
    }
    th->_AfterRun();
    return nullptr;
}

}
