#ifndef __MZX_TIMER_H__
#define __MZX_TIMER_H__

#include <cstdint>
#include <functional>
#include <list>
#include <vector>

namespace mzx
{

using TimerID = std::int64_t;
constexpr TimerID TIMER_ID_INVALID = (TimerID)-1;

struct TimerBase;
class Timer final
{

public:
    explicit Timer(int64_t cur_time = 0);
    ~Timer();
    Timer(const Timer &) = delete;
    Timer &operator=(const Timer &) = delete;

public:
    TimerID SetTimer(std::function<void()> cb, int64_t delay = 0,
                     int64_t interval = 0);
    void DelTimer(TimerID id);
    int64_t ExpireTime(TimerID id);
    int64_t LeftTime(TimerID id);
    void Update(int64_t now_time);

private:
    TimerBase *GetFreeTimer();
    TimerBase *FindTimer(TimerID id);
    void InsertToWheel(TimerBase *timer);
    void CascadeTimer(std::size_t i);

private:
    int64_t cur_time_{0};
    int64_t next_time_{0};
    std::vector<TimerBase *> timer_list_;
    std::list<TimerBase *> timer_free_list_;
    TimerBase **timer_wheel_list_{nullptr};
};

} // namespace mzx

#endif
