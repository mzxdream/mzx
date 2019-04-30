#ifndef __MZX_SHELL_SORT_H__
#define __MZX_SHELL_SORT_H__

#include <mzx/logger.h>

namespace mzx {

template <typename RandIt, typename Compare>
void ShellSort(RandIt begin, RandIt end, Compare comp)
{
    MZX_CHECK_STATIC((std::is_same<std::random_access_iterator_tag
            , typename std::iterator_traits<RandIt>::iterator_category>::value));
    MZX_CHECK(end > begin);
    for (auto gap = (end - begin) / 2; gap > 0; gap >>= 1)
    {
        for (auto iter = begin + gap; iter < end; iter += gap)
        {
            auto tmp = std::move(*iter);
            auto iter_insert = iter;
            for (; iter_insert > begin; iter_insert -= gap)
            {
                if (!comp(*iter_insert, *(iter_insert - gap)))
                {
                    break;
                }
                *iter_insert = std::move(*(iter_insert - gap));
            }
            *iter_insert = std::move(tmp);
        }
    }
}

template <typename RandIt>
void ShellSort(RandIt begin, RandIt end)
{
    ShellSort(begin, end, std::less<decltype(*begin)>());
}

}

#endif
