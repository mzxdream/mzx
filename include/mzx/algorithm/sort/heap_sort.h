#ifndef __MZX_HEAP_SORT_H__
#define __MZX_HEAP_SORT_H__

#include <mzx/logger.h>

namespace mzx {

template <typename RandIt, typename Compare>
void MakeHeap(RandIt begin, RandIt end, Compare comp)
{
}

template <typename RandIt, typename Compare>
void HeapSort(RandIt begin, RandIt end, Compare comp)
{
    MZX_CHECK_STATIC((std::is_same<std::random_access_iterator_tag
            , typename std::iterator_traits<RandIt>::iterator_category>::value));
    MZX_CHECK(end > begin);
}

template <typename RandIt>
void HeapSort(RandIt begin, RandIt end)
{
    HeapSort(begin, end, std::less<decltype(*begin)>());
}

}

#endif
