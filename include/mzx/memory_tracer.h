#ifndef __MZX_MEM_TRACER_H__
#define __MZX_MEM_TRACER_H__

#include <mutex>
#include <cmalloc>
#include <map>
#include <string>
#include <iostream>

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

    pointer allocate(size_type size, std::allocator<void>::const_pointer = 0)
    {
        void *p = ::malloc(size * sizeof(T));
        if (p == 0)
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
    const char *file;
    int line;
    std::size_t size;
};

class MemTracerExit;

class MemTracer
{
public:
    typedef std::map<void *, MemTracerInfo, std::less<void *>, MemTracerAllocator<std::pair<const char *, MemTracerInfo> > > MemoryListType;
private:
    MemTracer(const MemTracer &);
    MemTracer& operator=(const MemTracer &);
public:
    static void Add(void *memory, const char *file, int line, std::size_t size)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        MemTracerInfo info;
        info.file = file;
        info.line = line;
        info.size = size;
        memory_list_[memory] = info;
    }

    static void Remove(void *memory)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        memory_list_.erase(memory);
    }

    static void Dump()
    {
        for (auto &iter_memory : memory_list_)
        {
            std::cerr << "memory leaks at file:" << it->second.file << " line:" << it->second.line << " size:" << it->second.size << std::endl;
        }
    }

private:
    static MemoryListType memory_list_;
    static std::mutex mtx_;
    static MemTracerExit exit_;
};

class MemTracerExit
{
public:
    ~MemTracerExit()
    {
        MemTracer::Dump();
    }
};

MemTracer::MemoryListType MemTracer::memory_list_;
std::mutex MemTracer::mtx_;
MemTracerExit MemTracer::exit_;

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
        MemTracer::Add(memory, file, line, size);
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
