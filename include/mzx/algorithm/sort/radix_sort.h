#ifndef __MZX_RADIX_SORT_H__
#define __MZX_RADIX_SORT_H__

#include <array>
#include <vector>

#include <mzx/logger.h>

namespace mzx
{

template <typename RandIt, typename Compare>
void RadixSort(RandIt begin, RandIt end, Compare comp)
{
    MZX_CHECK_STATIC(
        std::is_same<std::random_access_iterator_tag, typename std::iterator_traits<RandIt>::iterator_category>::value);
    MZX_CHECK_STATIC(std::is_integral<typename std::decay<decltype(*begin)>::type>::value);
    MZX_CHECK(end > begin);
    using T = typename std::decay<decltype(*begin)>::type;

    T p = 10;
    T d = 1;
    for (auto iter = begin; iter != end; ++iter)
    {
        MZX_CHECK(*iter >= 0);
        while (*iter >= p)
        {
            p *= 10;
            ++d;
        }
    }
    T radix = 1;
    std::array<int, 10> counter;
    std::vector<T> tmp(end - begin);
    for (T i = 1; i <= d; ++i)
    {
        counter.fill(0);
        for (auto iter = begin; iter != end; ++iter)
        {
            ++counter[(*iter / radix) % 10];
        }
        for (int j = 1; j < 10; ++j)
        {
            counter[j] += counter[j - 1];
        }
        for (auto iter = end; iter != begin; --iter)
        {
            tmp[--counter[*(iter - 1) / radix % 10]] = std::move(*(iter - 1));
        }
        auto iter_tmp = tmp.begin();
        for (auto iter = begin; iter != end; ++iter)
        {
            *iter = std::move(*iter_tmp++);
        }
        radix *= 10;
    }
}

template <typename RandIt>
void RadixSort(RandIt begin, RandIt end)
{
    RadixSort(begin, end, std::less<decltype(*begin)>());
}

} // namespace mzx

#endif
