#ifndef __MZX_COUNT_SORT_H__
#define __MZX_COUNT_SORT_H__

#include <mzx/logger.h>

namespace mzx
{

template <typename RandIt, typename Compare>
void CountSort(RandIt begin, RandIt end, Compare comp)
{
    MZX_CHECK_STATIC(
        std::is_same<std::random_access_iterator_tag, typename std::iterator_traits<RandIt>::iterator_category>::value);
    MZX_CHECK_STATIC(std::is_integral<decltype(*begin - *begin)>::value && "123123123");
    MZX_CHECK(end > begin);
}

template <typename RandIt>
void CountSort(RandIt begin, RandIt end)
{
    CountSort(begin, end, std::less<decltype(*begin)>());
}

} // namespace mzx

#endif
