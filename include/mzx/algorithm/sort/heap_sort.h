#ifndef __MZX_HEAP_SORT_H__
#define __MZX_HEAP_SORT_H__

#include <mzx/logger.h>

namespace mzx
{

template <typename RandIt, typename Compare, typename Index>
void __AdjustHeap(RandIt begin, RandIt end, Index idx, Compare comp)
{
    auto swap_idx = idx;
    auto child_idx = (idx << 1) + 1;
    if (child_idx < end - begin && comp(*(begin + swap_idx), *(begin + child_idx)))
    {
        swap_idx = child_idx;
    }
    child_idx = (idx << 1) + 2;
    if (child_idx < end - begin && comp(*(begin + swap_idx), *(begin + child_idx)))
    {
        swap_idx = child_idx;
    }
    if (swap_idx != idx)
    {
        std::iter_swap(begin + swap_idx, begin + idx);
        __AdjustHeap(begin, end, swap_idx, comp);
    }
}

template <typename RandIt, typename Compare>
void HeapSort(RandIt begin, RandIt end, Compare comp)
{
    MZX_CHECK_STATIC(
        std::is_same<std::random_access_iterator_tag, typename std::iterator_traits<RandIt>::iterator_category>::value);
    MZX_CHECK(end > begin);
    auto len = end - begin;
    for (auto i = (len - 1) >> 1; i >= 0; --i)
    {
        __AdjustHeap(begin, end, i, comp);
    }
    for (auto iter = end - 1; iter != begin; --iter)
    {
        std::iter_swap(iter, begin);
        __AdjustHeap(begin, iter, 0, comp);
    }
}

template <typename RandIt>
void HeapSort(RandIt begin, RandIt end)
{
    HeapSort(begin, end, std::less<decltype(*begin)>());
}

} // namespace mzx

#endif
