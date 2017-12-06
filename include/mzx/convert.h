#ifndef __MZX_CONVERT_H__
#define __MZX_CONVERT_H__

#include <sstream>
#include <string>

namespace mzx {

template <typename Target, typename Source>
inline Target ConvertTo(const Source &src, const Target &def_tgt = Target())
{
    std::stringstream ss;
    Target tgt;
    if (!(ss << src) || !(ss >> tgt))
    {
        return def_tgt;
    }
    return tgt;
}

}

#endif
