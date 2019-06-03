#ifndef __MZX_TIMER_H__
#define __MZX_TIMER_H__

#include <cstdint>
#include <deque>
#include <functional>

#include <mzx/data_structure/list.h>

namespace mzx
{

using TimerID = std::size_t;
constexpr TimerID TIMER_ID_INVALID = (TimerID)-1;

class TimerManager final
{
public:
    TimerManager();
    ~TimerManager();
    TimerManager(const TimerManager &) = delete;
    TimerManager &operator=(const TimerManager &) = delete;

public:
    bool Init(int64_t cur_time);
    void Uninit();
    int64_t LeftTime(TimerID id) const;
    TimerID SetTimer(std::function<void()> cb, int64_t delay = 0,
                     int64_t interval = 0);
    void DelTimer(TimerID id);
    void Update(int64_t cur_time);

private:
    int64_t cur_time_{0};
    int64_t next_time_{0};
    std::deque<ListNode *> node_list_;
    List free_list_;
    List **wheel_list_{nullptr};
};

} // namespace mzx

#endif
