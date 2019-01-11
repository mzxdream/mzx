#ifndef __MZX_CONVERT_H__
#define __MZX_CONVERT_H__

#include <sstream>
#include <string>
#include <mzx/logger.h>

namespace mzx {

template <typename Target, typename Source>
inline bool ConvertTo(Source &&src, Target *tgt)
{
    MZX_CHECK(tgt != nullptr);
    std::stringstream ss;
    return (ss << std::forward<Source>(src)) && (ss >> *tgt) && ss.eof();
}

template <typename Target, typename Source>
inline Target ConvertTo(Source &&src)
{
    std::stringstream ss;
    Target tgt;
    if (!(ss << std::forward<Source>(src)) || !(ss >> tgt) || !ss.eof())
    {
        MZX_CHECK(false, "convert failed");
    }
    return tgt;
}

}

#endif
