#ifndef __MZX_INSERT_SORT_H__
#define __MZX_INSERT_SORT_H__

#include <mzx/logger.h>

namespace mzx
{

template <typename RandIt, typename Compare>
void InsertSort(RandIt begin, RandIt end, Compare comp)
{
    MZX_CHECK_STATIC(
        std::is_same<
            std::random_access_iterator_tag,
            typename std::iterator_traits<RandIt>::iterator_category>::value);
    MZX_CHECK(end > begin);
    for (auto iter = begin + 1; iter != end; ++iter)
    {
        auto tmp = std::move(*iter);
        auto iter_insert = iter;
        for (; iter_insert != begin; --iter_insert)
        {
            if (!comp(tmp, *(iter_insert - 1)))
            {
                break;
            }
            *iter_insert = std::move(*(iter_insert - 1));
        }
        *iter_insert = std::move(tmp);
    }
}

template <typename RandIt>
void InsertSort(RandIt begin, RandIt end)
{
    InsertSort(begin, end, std::less<decltype(*begin)>());
}

} // namespace mzx

#endif
