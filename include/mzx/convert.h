#ifndef __MZX_CONVERT_H__
#define __MZX_CONVERT_H__

#include <sstream>
#include <string>

#include <mzx/assert.h>

namespace mzx {

template <typename Target, typename Source>
inline Target ConvertTo(const Source &src, const Target &def_tgt)
{
    std::stringstream ss;
    Target tgt;
    if (!(ss << src) || !(ss >> tgt))
    {
        return def_tgt;
    }
    return tgt;
}

template <typename Target, typename Source>
inline Target ConvertTo(const Source &src)
{
    std::stringstream ss;
    Target tgt;
    if (!(ss << src) || !(ss >> tgt))
    {
        Assert(false, "convert failed");
    }
    return tgt;
}

}

#endif
