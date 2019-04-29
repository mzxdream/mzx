#ifndef __MZX_SELECT_SORT_H__
#define __MZX_SELECT_SORT_H__

#include <mzx/logger.h>

namespace mzx {

template <typename RandIt, typename Compare>
void SelectSort(RandIt begin, RandIt end, Compare comp)
{
    MZX_CHECK_STATIC((std::is_same<std::random_access_iterator_tag
            , typename std::iterator_traits<RandIt>::iterator_category>::value));
    MZX_CHECK(end > begin);
    for (auto iter = begin; iter != end - 1; ++iter)
    {
        for (auto iter_sel = iter + 1; iter_sel != end; ++iter_sel)
        {
            if (comp(*iter_sel, *iter))
            {
                std::iter_swap(iter_sel, iter);
            }
        }
    }
}

template <typename RandIt>
void SelectSort(RandIt begin, RandIt end)
{
    SelectSort(begin, end, std::less<decltype(*begin)>());
}

}

#endif
