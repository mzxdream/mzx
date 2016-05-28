#ifndef _M_THREAD_H_
#define _M_THREAD_H_

#include <pthread.h>
#include <string>
#include <mzx/util/m_errno.h>

typedef pthread_t MThreadType;

class MThread
{
protected:
    MThread();
    ~MThread();
    MThread(const MThread &) = delete;
    MThread& operator=(const MThread &) = delete;
public:
    MError Start();
    void Stop();
    MError Join();
    MError StopAndJoin();
    MThreadType GetPID() const;
    static MThreadType GetCurrentPID();
private:
    virtual bool _BeforeRun() { return true; }
    virtual void _AfterRun() {}
    virtual void _Run() = 0;
private:
    static void* ThreadMain(void *p_param);
private:
    MThreadType tid_;
    bool stop_flag_;
};

#endif
