#ifndef __MZX_ECS_COMPONENT_H__
#define __MZX_ECS_COMPONENT_H__

#include <cstddef>
#include <utility>

namespace mzx {
namespace ecs {

class Entity;

class ComponentBase
{
    friend class Entity;
public:
    typedef std::size_t ClassIndexType;
private:
    virtual ~ComponentBase() = 0;
protected:
    static ClassIndexType next_class_index_;
};

template <typename T>
class Component
    : public ComponentBase
{
    friend class Entity;
public:
    const static ClassIndexType CLASS_INDEX = ++ComponentBase::next_class_index_;
private:
    template <typename ...Args>
    explicit Component(Args && ...args)
        : data_(std::forward<Args>(args)...)
    {
    }
    virtual ~Component()
    {
    }
    Component(const Component &) = delete;
    Component & operator=(const Component &) = delete;
public:
    T & Data() const
    {
        return data_;
    }
private:
    T data_;
};


template <typename T>
class ComponentHandler
{
public:
    explicit ComponentHandler(T *component = nullptr)
        : component_(component)
    {
    }
public:
    bool Valid() const
    {
        return !component_;
    }
    operator bool() const
    {
        return Valid();
    }
    T *operator->() const
    {
        return component_;
    }
    T &operator*() const
    {
        return *component_;
    }
private:
    T *component_;
};

}
}

#endif
