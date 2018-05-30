#include <cassert>
#include <array>
#include <mzx/timer.h>
#include <mzx/make_array.h>

namespace mzx {

constexpr std::size_t TIMER_WHEEL_BITS[] = {8, 6, 6, 6, 6};
constexpr auto TIMER_WHEEL_COUNT = sizeof(TIMER_WHEEL_BITS) / sizeof(TIMER_WHEEL_BITS[0]);
constexpr std::size_t CalculateTimerWheelSize(std::size_t i)
{
    return 1 << TIMER_WHEEL_BITS[i];
}
constexpr auto TIMER_WHEEL_SIZE = MakeArray<TIMER_WHEEL_COUNT>(CalculateTimerWheelSize);
constexpr std::size_t CalculateTimerWheelMask(std::size_t i)
{
    return TIMER_WHEEL_SIZE[i] - 1;
}
constexpr auto TIMER_WHEEL_MASK = MakeArray<TIMER_WHEEL_COUNT>(CalculateTimerWheelMask);
constexpr std::size_t CalculateTimerWheelOffset(std::size_t i)
{
    return i == 0 ? 0 : TIMER_WHEEL_BITS[i - 1] + CalculateTimerWheelOffset(i - 1);
}
constexpr auto TIMER_WHEEL_OFFSET = MakeArray<TIMER_WHEEL_COUNT>(CalculateTimerWheelOffset);

struct TimerBase
{
    TimerBase()
        : id(TIMER_ID_INVALID)
        , expire_time(0)
        , interval(0)
        , prev(nullptr)
        , next(nullptr)
    {
    }
    TimerID id;
    int64_t expire_time;
    int64_t interval;
    std::function<void ()> cb;
    TimerBase *prev;
    TimerBase *next;
};

static void ListTimerInsertTail(TimerBase *timer, TimerBase *head)
{
    timer->next = head;
    timer->prev = head->prev;
    head->prev->next = timer;
    head->prev = timer;
}

static TimerBase * ListTimerPopFront(TimerBase *head)
{
    if (!head || head->next == head)
    {
        return nullptr;
    }
    auto p = head->next;
    p->next->prev = head;
    head->next = p->next;
    p->prev = nullptr;
    p->next = nullptr;
    return p;
}

Timer::Timer(int64_t cur_time)
    : cur_time_(cur_time)
    , timer_wheel_list_(nullptr)
{
    timer_wheel_list_ = new TimerBase *[TIMER_WHEEL_COUNT];
    for (std::size_t i = 0; i < TIMER_WHEEL_COUNT; ++i)
    {
        timer_wheel_list_[i] = new TimerBase[TIMER_WHEEL_SIZE[i]];
        for (std::size_t j = 0; j < TIMER_WHEEL_SIZE[i]; ++j)
        {
            auto timer_head = &timer_wheel_list_[i][j];
            timer_head->prev = timer_head;
            timer_head->next = timer_head;
        }
    }
}

Timer::~Timer()
{
    for (auto &timer : timer_list_)
    {
        delete timer;
    }
    timer_list_.clear();
    timer_free_list_.clear();
    for (std::size_t i = 0; i < TIMER_WHEEL_COUNT; ++i)
    {
        delete [] timer_wheel_list_[i];
        timer_wheel_list_[i] = nullptr;
    }
    delete [] timer_wheel_list_;
    timer_wheel_list_ = nullptr;
}

TimerBase * Timer::GetFreeTimer()
{
    if (!timer_free_list_.empty())
    {
        TimerBase *timer = timer_free_list_.front();
        timer_free_list_.pop_front();
        timer->id |= static_cast<TimerID>((timer->id + 1) & 0xFFFFFFFF);
        return timer;
    }
    TimerBase *timer = new TimerBase();
    timer->id = static_cast<TimerID>(timer_list_.size() << 32);
    timer_list_.push_back(timer);
    return timer;
}

TimerBase * Timer::FindTimer(TimerID id)
{
    if (id == TIMER_ID_INVALID)
    {
        return nullptr;
    }
    std::size_t i = id >> 32;
    if (i >= timer_list_.size())
    {
        return nullptr;
    }
    auto timer = timer_list_[i];
    if (id != timer->id)
    {
        return nullptr;
    }
    return timer;
}

void Timer::InsertToWheel(TimerBase *timer)
{
    assert(timer != nullptr && "timer is nullptr");
    int64_t delay = timer->expire_time - cur_time_;
    if (delay <= 0)
    {
        ListTimerInsertTail(timer, &timer_wheel_list_[0][cur_time_ & TIMER_WHEEL_MASK[0]]);
        return;
    }
    for (std::size_t i = 0; i < TIMER_WHEEL_COUNT; ++i)
    {
        if (static_cast<std::size_t>(delay >> TIMER_WHEEL_OFFSET[i]) < TIMER_WHEEL_SIZE[i])
        {
            auto index = (timer->expire_time >> TIMER_WHEEL_OFFSET[i]) & TIMER_WHEEL_MASK[i];
            ListTimerInsertTail(timer, &timer_wheel_list_[i][index]);
            return;
        }
    }
    auto i = TIMER_WHEEL_COUNT - 1;
    auto index = ((cur_time_ >> TIMER_WHEEL_OFFSET[i]) + (TIMER_WHEEL_MASK[i])) & TIMER_WHEEL_MASK[i];
    ListTimerInsertTail(timer, &timer_wheel_list_[i][index]);
}

TimerID Timer::SetTimer(std::function<void ()> cb, int64_t delay, int64_t interval)
{
    if (!cb)
    {
        return TIMER_ID_INVALID;
    }
    auto timer = GetFreeTimer();
    if (!timer)
    {
        return TIMER_ID_INVALID;
    }
    timer->expire_time = cur_time_ + delay;
    timer->interval = interval;
    timer->cb = cb;
    InsertToWheel(timer);
    return timer->id;
}

void Timer::DelTimer(TimerID id)
{
    auto timer = FindTimer(id);
    if (!timer)
    {
        return;
    }
    timer->interval = 0;
    if (timer->prev != nullptr && timer->next != nullptr)
    {
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        timer->prev = nullptr;
        timer->next = nullptr;
        timer_free_list_.push_back(timer);
    }
}

int64_t Timer::ExpireTime(TimerID id)
{
    auto timer = FindTimer(id);
    if (!timer)
    {
        return -1;
    }
    if (timer->prev == nullptr || timer->next == nullptr)
    {
        return -1;
    }
    return timer->expire_time;
}

int64_t Timer::LeftTime(TimerID id)
{
    auto timer = FindTimer(id);
    if (!timer)
    {
        return -1;
    }
    if (timer->prev == nullptr || timer->next == nullptr)
    {
        return -1;
    }
    if (timer->expire_time <= cur_time_)
    {
        return 0;
    }
    return timer->expire_time - cur_time_;
}

void Timer::CascadeTimer(std::size_t i)
{
    if (i >= TIMER_WHEEL_COUNT)
    {
        return;
    }
    auto index = (cur_time_ >> TIMER_WHEEL_OFFSET[i]) & TIMER_WHEEL_MASK[i];
    auto head = &timer_wheel_list_[i][index];
    TimerBase *timer = nullptr;
    while ((timer = ListTimerPopFront(head)) != nullptr)
    {
        InsertToWheel(timer);
    }
    if (index == 0)
    {
        CascadeTimer(i + 1);
    }
}

void Timer::Update(int64_t now_time)
{
    while (cur_time_ < now_time)
    {
        auto index = cur_time_ & TIMER_WHEEL_MASK[0];
        if (index == 0)
        {
            CascadeTimer(1);
        }
        auto head = &timer_wheel_list_[0][index];
        TimerBase *timer = nullptr;
        while ((timer = ListTimerPopFront(head)) != nullptr)
        {
            (timer->cb)();
            if (timer->interval > 0)
            {
                timer->expire_time = cur_time_ + timer->interval;
                InsertToWheel(timer);
            }
            else
            {
                timer_free_list_.push_back(timer);
            }
        }
        ++cur_time_;
    }
}

}
