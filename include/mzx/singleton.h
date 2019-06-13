#ifndef __MZX_SINGLETON_H__
#define __MZX_SINGLETON_H__

#include <mutex>

namespace mzx
{

template <typename T>
class Singleton
{
protected:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

public:
    static T &Instance()
    {
        static T instance;
        return instance;
    }
};

} // namespace mzx

#endif
