#ifndef __MZX_CONVERT_H__
#define __MZX_CONVERT_H__

#include <sstream>
#include <string>
#include <mzx/logger.h>

namespace mzx {

template <typename Target, typename Source>
bool ConvertTo(const Source &src, Target *tgt)
{
    std::stringstream ss;
    Target tmp;
    if (!(ss << src) || !(ss >> tmp) || !ss.eof())
    {
        return false;
    }
    if (tgt)
    {
        *tgt = tmp;
    }
    return true;
}

template <typename Target, typename Source>
Target ConvertTo(const Source &src)
{
    std::stringstream ss;
    Target tgt;
    if (!(ss << src) || !(ss >> tgt) || !ss.eof())
    {
        MZX_CHECK(false, "convert failed");
    }
    return tgt;
}

}

#endif
