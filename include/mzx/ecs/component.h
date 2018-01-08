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
    typedef std::size_t ID;
private:
    virtual ~ComponentBase() = 0;
protected:
    static ID next_id_;
};

template <typename T>
class Component
    : public ComponentBase
{
    friend class Entity;
private:
    template <typename ...Args>
    explicit Component(Args && ...args)
        : data_(new T(std::forward<Args>(args)...))
    {
    }
    virtual ~Component()
    {
        if (data_)
        {
            delete data_;
            data_ = nullptr;
        }
    }
    Component(const Component &) = delete;
    Component & operator=(const Component &) = delete;
private:
    T * Data() const
    {
        return data_;
    }
    static ID GetID()
    {
        static ID id = ++ComponentBase::next_id_;
        return id;
    }
private:
    T *data_;
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
