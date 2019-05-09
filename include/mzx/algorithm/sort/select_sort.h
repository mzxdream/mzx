#ifndef __MZX_SELECT_SORT_H__
#define __MZX_SELECT_SORT_H__

#include <mzx/logger.h>

namespace mzx
{

template <typename RandIt, typename Compare>
void SelectSort(RandIt begin, RandIt end, Compare comp)
{
    MZX_CHECK_STATIC(
        std::is_same<std::random_access_iterator_tag, typename std::iterator_traits<RandIt>::iterator_category>::value);
    MZX_CHECK(end > begin);
    for (auto iter = begin; iter != end - 1; ++iter)
    {
        auto iter_sel = iter;
        for (auto iter_tmp = iter + 1; iter_tmp != end; ++iter_tmp)
        {
            if (comp(*iter_tmp, *iter_sel))
            {
                iter_sel = iter_tmp;
            }
        }
        std::iter_swap(iter, iter_sel);
    }
}

template <typename RandIt>
void SelectSort(RandIt begin, RandIt end)
{
    SelectSort(begin, end, std::less<decltype(*begin)>());
}

} // namespace mzx

#endif
