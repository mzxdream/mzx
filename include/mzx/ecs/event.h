#ifndef __MZX_ECS_EVENT_H__
#define __MZX_ECS_EVENT_H__

#include <cstddef>
#include <functional>
#include <map>

namespace mzx {
namespace ecs {

class EventBase
{
public:
    typedef std::size_t ClassIndexType;
    typedef std::size_t ID;
public:
    virtual ~EventBase() = 0;
protected:
    static ClassIndexType next_class_index_;
};

template <typename ...Args>
class Event
    : public EventBase
{
public:
    typedef std::function<void (Args...)> Listener;
    const static ClassIndexType CLASS_INDEX = ++EventBase::next_class_index_;
public:
    Event()
    {
    }
    virtual ~Event()
    {
    }
    Event(const Event &) = delete;
    Event & operator=(const Event &) = delete;
public:
    ID AddListener(const Listener &listener)
    {
        ID id = ++next_id_;
        listener_list_[id] = listener;
        return id;
    }
    void RemoveListener(ID id)
    {
        listener_list_.erase(id);
    }
    void RemoveAllListener()
    {
        listener_list_.clear();
    }
    void Invoke(Args && ...args) const
    {
        for (auto &iter_listener : listener_list_)
        {
            (iter_listener.second)(std::forward(args)...);
        }
    }
private:
    std::map<ID, Listener> listener_list_;
    static ID next_id_;
};

template <typename ...Args>
typename Event<Args...>::ID Event<Args...>::next_id_ = 0;

}
}

#endif
