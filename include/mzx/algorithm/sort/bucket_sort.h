#ifndef __MZX_BUCKET_SORT_H__
#define __MZX_BUCKET_SORT_H__

#include <map>
#include <vector>

#include <mzx/algorithm/sort/insert_sort.h>
#include <mzx/logger.h>

namespace mzx
{

template <typename RandIt, typename Compare>
void BucketSort(RandIt begin, RandIt end, Compare comp)
{
    MZX_CHECK_STATIC(
        std::is_same<
            std::random_access_iterator_tag,
            typename std::iterator_traits<RandIt>::iterator_category>::value);
    MZX_CHECK_STATIC(
        std::is_integral<typename std::decay<decltype(*begin)>::type>::value);
    MZX_CHECK(end > begin);
    using T = typename std::decay<decltype(*begin)>::type;
    T num = 10;
    std::map<T, std::vector<T>> buckets;
    for (auto iter = begin; iter != end; ++iter)
    {
        buckets[*iter / num].emplace_back(std::move(*iter));
    }
    auto iter_copy = begin;
    for (auto iter = buckets.begin(); iter != buckets.end(); ++iter)
    {
        if (iter->second.empty())
        {
            continue;
        }
        InsertSort(iter->second.begin(), iter->second.end(), comp);
        for (auto &tmp : iter->second)
        {
            *iter_copy++ = std::move(tmp);
        }
    }
}

template <typename RandIt>
void BucketSort(RandIt begin, RandIt end)
{
    BucketSort(begin, end, std::less<decltype(*begin)>());
}

} // namespace mzx

#endif
