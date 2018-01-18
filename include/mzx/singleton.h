#ifndef __MZX_SINGLETON_H__
#define __MZX_SINGLETON_H__

#include <mutex>
#include <mzx/null_mutex.h>

namespace mzx {

template <typename T, typename Mutex = NullMutex>
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
    static void DestroyInstance()
    {
        return instance_.DestoryInstance();
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
                std::lock_guard<Mutex> lock(mtx_);
                if (!instance_)
                {
                    instance_ = new T();
                }
            }
            return *instance_;
        }
        void DestroyInstance()
        {
            if (instance_)
            {
                std::lock_guard<Mutex> lock(mtx_);
                if (instance_)
                {
                    delete instance_;
                    instance_ = nullptr;
                }
            }
        }
    private:
        T *instance_;
        Mutex mtx_;
    };
private:
    static SingletonBase instance_;
};

template<typename T, typename Mutex>
typename Singleton<T, Mutex>::SingletonBase Singleton<T, Mutex>::instance_;

}

#endif
