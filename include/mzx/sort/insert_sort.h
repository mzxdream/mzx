#ifndef __MZX_INSERT_SORT_H__
#define __MZX_INSERT_SORT_H__

#include <mzx/logger.h>

namespace mzx {

template <typename RandIt, typename Compare>
void InsertSort(RandIt begin, RandIt end, Compare comp)
{
    MZX_CHECK_STATIC((std::is_same<std::random_access_iterator_tag
            , typename std::iterator_traits<RandIt>::iterator_category>::value));
    MZX_CHECK(end > begin);
    for (auto iter = begin + 1; iter != end; ++iter)
    {
        for (auto iter_check = iter; iter_check != begin; --iter_check)
        {
            if (!comp(*iter_check, *(iter_check - 1)))
            {
                break;
            }
            std::iter_swap(iter_check, iter_check - 1);
        }
    }
}

}

#endif
