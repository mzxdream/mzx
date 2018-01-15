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
    typedef std::size_t IDType;
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
public:
    IDType AddListener(const Listener &listener)
    {
        IDType id = ++next_id_;
        listener_list_[id] = listener;
        return id;
    }
    void RemoveListener(IDType id)
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
    std::map<IDType, Listener> listener_list_;
    static IDType next_id_;
};

template <typename ...Args>
typename Event<Args...>::IDType Event<Args...>::next_id_ = 0;

}
}

#endif
