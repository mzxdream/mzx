#ifndef __MZX_MEM_TRACER_H__
#define __MZX_MEM_TRACER_H__

#include <mutex>
#include <cstdlib>
#include <map>
#include <string>
#include <cstdio>
#include <functional>

namespace mzx {

template <typename T>
struct MemTracerAllocator
    : std::allocator<T>
{
    typedef typename std::allocator<T>::pointer pointer;
    typedef typename std::allocator<T>::size_type size_type;

    template <typename U>
    struct rebind
    {
        typedef MemTracerAllocator<U> other;
    };

    MemTracerAllocator()
    {
    }

    template <typename U>
    MemTracerAllocator(MemTracerAllocator<U> const &u)
        : std::allocator<T>(u)
    {
    }

    pointer allocate(size_type size, std::allocator<void>::const_pointer = nullptr)
    {
        void *p = ::malloc(size * sizeof(T));
        if (p == nullptr)
        {
            throw std::bad_alloc();
        }
        return static_cast<pointer>(p);
    }

    void deallocate(pointer p, size_type)
    {
        ::free(p);
    }
};

struct MemTracerInfo
{
    MemTracerInfo() = default;
    explicit MemTracerInfo(const char *f, int l, void *m, std::size_t s)
        : file(f)
        , line(l)
        , memory(m)
        , size(s)
    {
    }
    const char *file{ nullptr };
    int line{ 0 };
    void *memory{ nullptr };
    std::size_t size{ 0 };
};

class MemTracerExit;

class MemTracer
{
public:
    using MemoryListType = std::map<void *, MemTracerInfo, std::less<void *>, MemTracerAllocator<std::pair<void *, MemTracerInfo> > >;
    using DumpHandler = std::function<void (const MemTracerInfo &)>;
    using ExitHandler = std::function<void ()>;
private:
    MemTracer(const MemTracer &);
    MemTracer& operator=(const MemTracer &);
public:
    static void SetExitHandler(ExitHandler handler)
    {
        exit_handler_ = handler;
    }

    static void Add(void *memory, const char *file, int line, std::size_t size)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (memory_list_.find(memory) != memory_list_.end())
        {
            std::abort();
        }
        memory_list_.emplace(memory, MemTracerInfo(file, line, memory, size));
    }

    static void Remove(void *memory)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        memory_list_.erase(memory);
    }

    static void Dump(DumpHandler handler)
    {
        if (!handler)
        {
            return;
        }
        std::lock_guard<std::mutex> lock(mtx_);
        for (auto &it : memory_list_)
        {
            handler(it.second);
        }
    }

    static void OnExit()
    {
        if (exit_handler_)
        {
            exit_handler_();
        }
    }
private:
    static MemoryListType memory_list_;
    static std::mutex mtx_;
    static ExitHandler exit_handler_;
    static MemTracerExit exit_;
};

class MemTracerExit
{
public:
    ~MemTracerExit()
    {
        MemTracer::OnExit();
    }
};

}

inline void * operator new(std::size_t size, const char *file, int line)
{
    void *memory = ::malloc(size);
    if (memory)
    {
        mzx::MemTracer::Add(memory, file, line, size);
    }
    return memory;
}

inline void operator delete(void *memory, const char *file, int line)
{
    if (memory)
    {
        mzx::MemTracer::Remove(memory);
        ::free(memory);
    }
}

inline void* operator new(size_t size)
{
    void *memory = ::malloc(size);
    if (memory)
    {
        mzx::MemTracer::Add(memory, "?", 0, size);
    }
    return memory;
}

inline void operator delete(void *memory)
{
    if (memory)
    {
        mzx::MemTracer::Remove(memory);
        ::free(memory);
    }
}

inline void* operator new[](size_t size, const char *file, int line)
{
    void *memory = ::malloc(size);
    if (memory)
    {
        mzx::MemTracer::Add(memory, file, line, size);
    }
    return memory;
}

inline void operator delete[](void *memory, const char *file, int line)
{
    if (memory)
    {
        mzx::MemTracer::Remove(memory);
        ::free(memory);
    }
}

inline void* operator new[](size_t size)
{
    void *memory = ::malloc(size);
    if (memory)
    {
        mzx::MemTracer::Add(memory, "?", 0, size);
    }
    return memory;
}

inline void operator delete[](void *memory)
{
    if (memory)
    {
        mzx::MemTracer::Remove(memory);
        ::free(memory);
    }
}

inline void* _malloc(size_t size, const char *file, int line)
{
    void *memory = ::malloc(size);
    if (memory)
    {
        mzx::MemTracer::Add(memory, file, line, size);
    }
    return memory;
}

inline void* _calloc(size_t n, size_t size, const char *file, int line)
{
    void *memory = ::calloc(n, size);
    if (memory)
    {
        mzx::MemTracer::Add(memory, file, line, size);
    }
    return memory;
}

inline void* _realloc(void *memory, size_t size, const char *file, int line)
{
    if (memory)
    {
        mzx::MemTracer::Remove(memory);
    }
    memory = ::realloc(memory, size);
    if (memory)
    {
        mzx::MemTracer::Add(memory, file, line, size);
    }
    return memory;
}

inline void _free(void *memory)
{
    if (memory)
    {
        mzx::MemTracer::Remove(memory);
        ::free(memory);
    }
}

#define new            new(__FILE__, __LINE__)
#define malloc(s)      _malloc(s, __FILE__, __LINE__)
#define calloc(n, s)   _calloc(n, s, __FILE__, __LINE__)
#define realloc(p, s)  _realloc(p, s, __FILE__, __LINE__)
#define free(p)        _free(p)

#endif
