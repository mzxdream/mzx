#ifndef __MZX_SINGLETON_H__
#define __MZX_SINGLETON_H__

#include <mutex>
#include <mzx/null_mutex.h>

namespace mzx {

template <typename T, typename Mtx = NullMutex>
class Singleton
{
protected:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(const Singleton &) = delete;
    Singleton& operator=(const Singleton &) = delete;
public:
    static T& Instance()
    {
        return instance_.Instance();
    }
private:
    class SingletonBase
    {
    public:
        SingletonBase()
            :instance_(nullptr)
        {
        }
        ~SingletonBase()
        {
            if (instance_)
            {
                delete instance_;
                instance_ = nullptr;
            }
        }
        SingletonBase(const SingletonBase &) = delete;
        SingletonBase& operator=(const Singleton &) = delete;
    public:
        T& Instance()
        {
            if (!instance_)
            {
                std::lock_guard<Mtx> lock(mtx_);
                if (!instance_)
                {
                    instance_ = new T();
                }
            }
            return *instance_;
        }
    private:
        T *instance_;
        Mtx mtx_;
    };
private:
    static SingletonBase instance_;
};

template<typename T, typename Mtx>
typename Singleton<T, Mtx>::SingletonBase Singleton<T, Mtx>::instance_;

}

#endif
