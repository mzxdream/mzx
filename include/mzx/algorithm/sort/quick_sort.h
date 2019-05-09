#ifndef __MZX_QUICK_SORT_H__
#define __MZX_QUICK_SORT_H__

#include <mzx/logger.h>

namespace mzx
{

template <typename RandIt, typename Compare>
void __QuickSort(RandIt begin, RandIt end, Compare comp)
{
    if (end - begin <= 1)
    {
        return;
    }
    auto tmp = std::move(*begin);
    auto iter_first = begin;
    auto iter_last = end - 1;
    while (iter_first < iter_last)
    {
        while (comp(tmp, *iter_last) && iter_first < iter_last)
        {
            --iter_last;
        }
        if (iter_first < iter_last)
        {
            *iter_first++ = std::move(*iter_last);
        }
        while (comp(*iter_first, tmp) && iter_first < iter_last)
        {
            ++iter_first;
        }
        if (iter_first < iter_last)
        {
            *iter_last-- = std::move(*iter_first);
        }
    }
    *iter_first = std::move(tmp);
    __QuickSort(begin, iter_first, comp);
    __QuickSort(iter_first + 1, end, comp);
}

template <typename RandIt, typename Compare>
void QuickSort(RandIt begin, RandIt end, Compare comp)
{
    MZX_CHECK_STATIC(
        std::is_same<std::random_access_iterator_tag, typename std::iterator_traits<RandIt>::iterator_category>::value);
    MZX_CHECK(end > begin);
    __QuickSort(begin, end, comp);
}

template <typename RandIt>
void QuickSort(RandIt begin, RandIt end)
{
    QuickSort(begin, end, std::less<decltype(*begin)>());
}

} // namespace mzx

#endif
