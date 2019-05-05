#ifndef __MZX_COUNT_SORT_H__
#define __MZX_COUNT_SORT_H__

#include <mzx/logger.h>

namespace mzx {

template <typename RandIt, typename Compare>
void CountSort(RandIt begin, RandIt end, Compare comp)
{
    MZX_CHECK_STATIC((std::is_same<std::random_access_iterator_tag
            , typename std::iterator_traits<RandIt>::iterator_category>::value));
    //MZX_CHECK_STATIC((std::is_integral<decltype(*begin - *begin)>::value));
    MZX_CHECK(end > begin);
    auto len = end - begin;
    auto iter_tmp = end - 1;
    for (decltype(len) i = 0; i < len - 1; ++i)
    {
        auto iter_flag = iter_tmp;
        for (auto iter = begin; iter != iter_flag; ++iter)
        {
            if (comp(*(iter + 1), *iter))
            {
                std::iter_swap(iter + 1, iter);
                iter_tmp = iter;
            }
        }
        if (iter_tmp == begin)
        {
            break;
        }
    }
}

template <typename RandIt>
void CountSort(RandIt begin, RandIt end)
{
    CountSort(begin, end, std::less<decltype(*begin)>());
}

}

#endif
