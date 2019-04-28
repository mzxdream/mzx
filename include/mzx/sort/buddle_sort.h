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
    std::sort(begin, end, comp);
}

template <typename RandIt>
void BuddleSort(RandIt begin, RandIt end)
{
    BuddleSort(begin, end, std::less<decltype(*begin)>());
}

}

#endif
