#ifndef __MZX_COUNT_SORT_H__
#define __MZX_COUNT_SORT_H__

#include <vector>

#include <mzx/logger.h>

namespace mzx
{

template <typename RandIt, typename Compare>
void CountSort(RandIt begin, RandIt end, Compare comp)
{
    MZX_CHECK_STATIC(
        std::is_same<std::random_access_iterator_tag, typename std::iterator_traits<RandIt>::iterator_category>::value);
    MZX_CHECK_STATIC(std::is_integral<decltype(*begin - *begin)>::value);
    MZX_CHECK(end > begin);
    auto min_index = begin;
    auto max_index = begin;
    for (auto iter = begin + 1; iter != end; ++iter)
    {
        if (comp(*iter, *min_index))
        {
            min_index = iter;
        }
        else if (comp(*max_index, *iter))
        {
            max_index = iter;
        }
    }
    std::vector<int> counter(*max_index - *min_index + 1, 0);
    for (auto iter = begin; iter != end; ++iter)
    {
        ++counter[*iter - *min_index];
    }
    for (auto iter = counter.begin() + 1; iter != counter.end(); ++iter)
    {
        *iter += *(iter - 1);
    }
    std::vector<typename std::decay<decltype(*begin)>::type> tmp(end - begin);
    for (auto iter = end; iter != begin; --iter)
    {
        tmp[--counter[*(iter - 1) - *min_index]] = std::move(*(iter - 1));
    }
    auto iter_tmp = tmp.begin();
    for (auto iter = begin; iter != end; ++iter)
    {
        *iter = std::move(*iter_tmp++);
    }
}

template <typename RandIt>
void CountSort(RandIt begin, RandIt end)
{
    CountSort(begin, end, std::less<decltype(*begin)>());
}

} // namespace mzx

#endif
