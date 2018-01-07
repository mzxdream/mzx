#ifndef __MZX_THREAD_H__
#define __MZX_THREAD_H__

#include <pthread.h>
#include <string>

namespace mzx {

class Thread
{
public:
    typedef pthread_t PID;
private:
    static const PID PID_INVALID = 0;
protected:
    Thread();
    ~Thread();
    Thread(const Thread &) = delete;
    Thread& operator=(const Thread &) = delete;
public:
    bool Start();
    void Stop();
    bool Join();
    bool StopAndJoin();
    PID GetPID() const;
    static PID GetCurPID();
private:
    virtual bool _BeforeRun()
    {
        return true;
    }
    virtual void _AfterRun()
    {
    }
    virtual void _Run() = 0;
private:
    static void* ThreadMain(void *p_param);
private:
    PID pid_;
    volatile bool stop_flag_;
};

}

#endif