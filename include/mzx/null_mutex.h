#ifndef __MZX_NULL_MUTEX_H__
#define __MZX_NULL_MUTEX_H__

namespace mzx
{

class NullMutex
{
public:
    void lock()
    {
    }
    void unlock()
    {
    }
    bool try_lock()
    {
        return true;
    }
};

} // namespace mzx

#endif
