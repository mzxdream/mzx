#ifndef __MZX_MERGE_SORT_H__
#define __MZX_MERGE_SORT_H__

#include <mzx/logger.h>
#include <vector>

namespace mzx
{

template <typename RandIt, typename RandItStore, typename Compare>
void __MergeSort(RandIt begin, RandIt end, RandItStore store, Compare comp)
{
    if (end - begin == 1)
    {
        return;
    }
    auto iter_middle = begin + (end - begin) / 2;
    __MergeSort(begin, iter_middle, store, comp);
    __MergeSort(iter_middle, end, store + (end - begin) / 2, comp);
    auto iter_first = begin;
    auto iter_second = iter_middle;
    auto iter_store = store;
    while (iter_first != iter_middle && iter_second != end)
    {
        if (comp(*iter_second, *iter_first))
        {
            *iter_store++ = std::move(*iter_second++);
        }
        else
        {
            *iter_store++ = std::move(*iter_first++);
        }
    }
    while (iter_first != iter_middle)
    {
        *iter_store++ = std::move(*iter_first++);
    }
    while (begin != iter_second)
    {
        *begin++ = std::move(*store++);
    }
}

template <typename RandIt, typename Compare>
void MergeSort(RandIt begin, RandIt end, Compare comp)
{
    MZX_CHECK_STATIC(
        std::is_same<
            std::random_access_iterator_tag,
            typename std::iterator_traits<RandIt>::iterator_category>::value);
    MZX_CHECK(end > begin);
    std::vector<typename std::remove_reference<decltype(*begin)>::type> tmp(
        end - begin);
    __MergeSort(begin, end, tmp.begin(), comp);
}

template <typename RandIt>
void MergeSort(RandIt begin, RandIt end)
{
    MergeSort(begin, end, std::less<decltype(*begin)>());
}

} // namespace mzx

#endif
