#include <mzx/data_structure/make_array.h>
#include <mzx/logger.h>
#include <mzx/macro_util.h>
#include <mzx/timer_manager.h>

namespace mzx
{

constexpr static std::size_t TIMER_WHEEL_BITS[] = {8, 6, 6, 6, 6};
constexpr static auto TIMER_WHEEL_COUNT =
    sizeof(TIMER_WHEEL_BITS) / sizeof(TIMER_WHEEL_BITS[0]);
constexpr static std::size_t CalculateTimerWheelSize(std::size_t i)
{
    return 1 << TIMER_WHEEL_BITS[i];
}
constexpr auto TIMER_WHEEL_SIZE =
    MakeArray<TIMER_WHEEL_COUNT>(CalculateTimerWheelSize);
constexpr std::size_t CalculateTimerWheelMask(std::size_t i)
{
    return TIMER_WHEEL_SIZE[i] - 1;
}
constexpr auto TIMER_WHEEL_MASK =
    MakeArray<TIMER_WHEEL_COUNT>(CalculateTimerWheelMask);
constexpr std::size_t CalculateTimerWheelOffset(std::size_t i)
{
    return i == 0 ? 0
                  : TIMER_WHEEL_BITS[i - 1] + CalculateTimerWheelOffset(i - 1);
}
constexpr auto TIMER_WHEEL_OFFSET =
    MakeArray<TIMER_WHEEL_COUNT>(CalculateTimerWheelOffset);

constexpr std::size_t TIMER_ID_COUNT_BIT = 16;
constexpr std::size_t TIMER_ID_COUNT_MASK =
    (static_cast<std::size_t>(1) << TIMER_ID_COUNT_BIT) - 1;

struct TimerNode
{
    TimerID id{TIMER_ID_INVALID};
    int64_t expire_time{0};
    int64_t interval{0};
    std::function<void()> cb;
    bool is_using{false};
    ListNode list_link;
};

TimerManager::TimerManager(int64_t cur_time)
    : cur_time_(cur_time)
    , next_time_(cur_time)
{
    wheel_list_ = new List *[TIMER_WHEEL_COUNT];
    for (std::size_t i = 0; i < TIMER_WHEEL_COUNT; ++i)
    {
        wheel_list_[i] = new List[TIMER_WHEEL_SIZE[i]];
    }
}

TimerManager::~TimerManager()
{
    for (auto &node : node_list_)
    {
        auto *timer_node = MZX_CONTAINER_OF(node, TimerNode, list_link);
        delete timer_node;
    }
    node_list_.clear();
    free_list_.Clear();
    for (std::size_t i = 0; i < TIMER_WHEEL_COUNT; ++i)
    {
        delete[] wheel_list_[i];
        wheel_list_[i] = nullptr;
    }
    delete[] wheel_list_;
    wheel_list_ = nullptr;
}

static TimerNode *FindUsingTimerNode(const std::deque<ListNode *> &node_list,
                                     TimerID id)
{
    if (id == TIMER_ID_INVALID)
    {
        return nullptr;
    }
    std::size_t i = static_cast<std::size_t>(id >> TIMER_ID_COUNT_BIT);
    if (i >= node_list.size())
    {
        return nullptr;
    }
    auto *node = node_list[i];
    auto *timer_node = MZX_CONTAINER_OF(node, TimerNode, list_link);
    return id == timer_node->id && timer_node->is_using ? timer_node : nullptr;
}

int64_t TimerManager::ExpireTime(TimerID id) const
{
    auto *timer_node = FindUsingTimerNode(node_list_, id);
    return timer_node ? timer_node->expire_time : -1;
}

int64_t TimerManager::LeftTime(TimerID id) const
{
    auto *timer_node = FindUsingTimerNode(node_list_, id);
    if (!timer_node)
    {
        return -1;
    }
    auto left_time = timer_node->expire_time - cur_time_;
    if (left_time < 0)
    {
        return 0;
    }
    return left_time;
}

static void InsertTimerNodeToWheel(int64_t cur_time, TimerNode *timer_node,
                                   List **wheel_list)
{
    MZX_CHECK(timer_node != nullptr && wheel_list != nullptr);
    timer_node->list_link.Erase();
    auto delay = timer_node->expire_time - cur_time;
    if (delay < 1)
    {
        wheel_list[0][(cur_time + 1) & TIMER_WHEEL_MASK[0]].PushBack(
            &timer_node->list_link);
        return;
    }
    std::size_t i = 0;
    for (; i < TIMER_WHEEL_COUNT; ++i)
    {
        if (static_cast<std::size_t>(delay >> TIMER_WHEEL_OFFSET[i]) <=
            TIMER_WHEEL_SIZE[i])
        {
            auto index = (timer_node->expire_time >> TIMER_WHEEL_OFFSET[i]) &
                         TIMER_WHEEL_MASK[i];
            wheel_list[i][index].PushBack(&timer_node->list_link);
            return;
        }
    }
    --i;
    auto index = (cur_time >> TIMER_WHEEL_OFFSET[i]) & TIMER_WHEEL_MASK[i];
    wheel_list[i][index].PushBack(&timer_node->list_link);
}

TimerID TimerManager::SetTimer(std::function<void()> cb, int64_t delay,
                               int64_t interval)
{
    MZX_CHECK(cb != nullptr);

    TimerNode *timer_node = nullptr;
    auto *node = free_list_.PopFront();
    if (node)
    {
        timer_node = MZX_CONTAINER_OF(node, TimerNode, list_link);
        timer_node->id =
            static_cast<TimerID>((timer_node->id & ~TIMER_ID_COUNT_MASK) |
                                 ((timer_node->id + 1) & TIMER_ID_COUNT_MASK));
    }
    else
    {
        timer_node = new TimerNode;
        timer_node->id =
            static_cast<TimerID>(node_list_.size() << TIMER_ID_COUNT_BIT);
        node_list_.emplace_back(&timer_node->list_link);
    }
    timer_node->expire_time = cur_time_ + delay;
    timer_node->interval = interval;
    timer_node->cb = cb;
    timer_node->is_using = true;
    InsertTimerNodeToWheel(cur_time_, timer_node, wheel_list_);
    return timer_node->id;
}

static void MoveTimerNodeToFreeList(TimerNode *timer_node, List *free_list)
{
    MZX_CHECK(timer_node != nullptr && timer_node->is_using &&
              free_list != nullptr);
    timer_node->list_link.Erase();
    timer_node->expire_time = 0;
    timer_node->interval = 0;
    timer_node->cb = nullptr;
    timer_node->is_using = false;
    free_list->PushBack(&timer_node->list_link);
}

void TimerManager::DelTimer(TimerID id)
{
    auto *timer_node = FindUsingTimerNode(node_list_, id);
    if (!timer_node)
    {
        return;
    }
    MoveTimerNodeToFreeList(timer_node, &free_list_);
}

static void TimerWheelCascade(int64_t cur_time, List **wheel_list,
                              std::size_t i)
{
    MZX_CHECK(wheel_list != nullptr);
    if (i >= TIMER_WHEEL_COUNT)
    {
        return;
    }
    auto index =
        ((cur_time >> TIMER_WHEEL_OFFSET[i]) + 1) & TIMER_WHEEL_MASK[i];
    auto &wheel = wheel_list[i][index];
    ListNode *node = nullptr;
    while ((node = wheel.PopFront()) != nullptr)
    {
        auto *timer_node = MZX_CONTAINER_OF(node, TimerNode, list_link);
        InsertTimerNodeToWheel(cur_time, timer_node, wheel_list);
    }
    if (index == 0)
    {
        TimerWheelCascade(cur_time, wheel_list, i + 1);
    }
}

void TimerManager::Update(int64_t now_time)
{
    if (now_time <= cur_time_)
    {
        return;
    }
    next_time_ = now_time;
    while (cur_time_ < next_time_)
    {
        ++cur_time_;
        auto index = cur_time_ & TIMER_WHEEL_MASK[0];
        auto &wheel = wheel_list_[0][index];
        ListNode *node = nullptr;
        while ((node = wheel.PopFront()) != nullptr)
        {
            auto *timer_node = MZX_CONTAINER_OF(node, TimerNode, list_link);
            (timer_node->cb)();
            if (!timer_node->is_using)
            {
                continue;
            }
            if (timer_node->interval > 0)
            {
                timer_node->expire_time = cur_time_ + timer_node->interval;
                InsertTimerNodeToWheel(cur_time_, timer_node, wheel_list_);
            }
            else
            {
                MoveTimerNodeToFreeList(timer_node, &free_list_);
            }
        }
        if (index == 0)
        {
            TimerWheelCascade(cur_time_, wheel_list_, 1);
        }
    }
}

} // namespace mzx
