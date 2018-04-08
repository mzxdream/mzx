#ifndef __MZX_EVENT_H__
#define __MZX_EVENT_H__

#include <cstddef>
#include <functional>
#include <map>

namespace mzx {

using EventID = std::size_t;
const static EventID EVENT_ID_INVALID = (EventID)-1;

template <typename ...Args>
class Event
{
public:
    typedef std::function<void (Args...)> Listener;
public:
    Event()
        : next_id_(0)
    {
    }
    ~Event()
    {
    }
    Event(const Event &) = delete;
    Event & operator=(const Event &) = delete;
public:
    EventID AddListener(const Listener &listener)
    {
        EventID id = ++next_id_;
        listener_list_[id] = listener;
        return id;
    }
    void RemoveListener(EventID id)
    {
        listener_list_.erase(id);
    }
    void RemoveAllListener()
    {
        listener_list_.clear();
    }
    void Invoke(Args ...args) const
    {
        auto iter_listener = listener_list_.begin();
        while (iter_listener != listener_list_.end())
        {
            auto &listener = iter_listener.second;
            ++iter_listener;
            if (listener)
            {
                listener(args...);
            }
        }
    }
private:
    std::map<EventID, Listener> listener_list_;
    EventID next_id_;
};

}

#endif
