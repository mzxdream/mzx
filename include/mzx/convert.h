#ifndef __MZX_CONVERT_H__
#define __MZX_CONVERT_H__

#include <sstream>
#include <string>

#include <mzx/logger.h>

namespace mzx
{

template <typename Target, typename Source>
inline bool TryConvertTo(Source &&src, Target *tgt)
{
    MZX_CHECK(tgt != nullptr);
    std::stringstream ss;
    return (ss << std::forward<Source>(src)) && (ss >> *tgt) && ss.eof();
}

template <typename Target, typename Source>
inline Target UnsafeConvertTo(Source &&src)
{
    std::stringstream ss;
    Target tgt;
    ss << std::forward<Source>(src);
    ss >> tgt;
    return tgt;
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

template <typename Target, typename Source>
inline Target ConvertTo(Source &&src, const Target &def_tgt)
{
    std::stringstream ss;
    Target tgt;
    if (!(ss << std::forward<Source>(src)) || !(ss >> tgt) || !ss.eof())
    {
        return def_tgt;
    }
    return tgt;
}

} // namespace mzx

#endif
