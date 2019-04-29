#ifndef __MZX_BUDDLE_SORT_H__
#define __MZX_BUDDLE_SORT_H__

#include <type_traits>
#include <algorithm>
#include <mzx/logger.h>

namespace mzx {

template <typename RandIt, typename Compare>
void BuddleSort(RandIt begin, RandIt end, Compare comp)
{
    MZX_CHECK_STATIC((std::is_same<std::random_access_iterator_tag
            , typename std::iterator_traits<RandIt>::iterator_category>::value));
    MZX_CHECK(end > begin);
    auto len = end - begin;
    auto iter_flag = begin;
    for (decltype(len) i = 0; i < len - 1; ++i)
    {
        auto iter_tmp = end - 1;
        for (auto iter = end - 1; iter != iter_flag; --iter)
        {
            if (comp(*iter, *(iter - 1)))
            {
                std::iter_swap(iter, iter - 1);
                iter_tmp = iter;
            }
        }
        if (iter_tmp == end - 1)
        {
            break;
        }
        iter_flag = iter_tmp;
    }
}

template <typename RandIt>
void BuddleSort(RandIt begin, RandIt end)
{
    BuddleSort(begin, end, std::less<decltype(*begin)>());
}

}

#endif
