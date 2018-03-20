#ifndef __MZX_COMPONENT_H__
#define __MZX_COMPONENT_H__

#include <cstddef>
#include <utility>

namespace mzx {

class ComponentBase
{
public:
    typedef std::size_t ClassIndexType;
public:
    virtual ~ComponentBase() = 0;
protected:
    static ClassIndexType next_class_index_;
};

template <typename T>
class Component
    : public ComponentBase
{
public:
    const static ClassIndexType CLASS_INDEX = ++ComponentBase::next_class_index_;
public:
    template <typename ...Args>
    explicit Component(Args && ...args)
        : data_(std::forward<Args>(args)...)
    {
    }
    virtual ~Component()
    {
    }
public:
    T * Data() const
    {
        return &data_;
    }
private:
    T data_;
};

template <typename T>
class ComponentHandle
{
public:
    explicit ComponentHandle(T *component = nullptr)
        : component_(component)
    {
    }
public:
    T * operator->() const
    {
        return component_;
    }
    operator bool() const
    {
        return IsValid();
    }
    bool IsValid() const
    {
        return component_ != nullptr;
    }
    T & Data() const
    {
        return *component_;
    }
private:
    T *component_;
};

}

#endif
