#ifndef __MZX_STRING_UTIL_H__
#define __MZX_STRING_UTIL_H__

#include <string>
#include <sstream>
#include <vector>

namespace mzx {

template <typename O>
inline void ConcatBase(O &os)
{
}

template <typename O, typename T, typename ...Args>
inline void ConcatBase(O &os, T &&val, Args && ...args)
{
    os << val;
    ConcatBase(os, std::forward<Args>(args)...);
}

template <typename ...Args>
inline std::string Concat(Args && ...args)
{
    std::stringstream ss;
    ConcatBase(ss, std::forward<Args>(args)...);
    return ss.str();
}

inline std::string TrimLeft(const std::string &str, char trim = ' ');
{
    auto pos = str.find_first_not_of(trim);
    if (pos != std::string::npos)
    {
        return str.substr(pos);
    }
    return std::string();
}

inline std::string TrimLeft(const std::string &str, const char *trim);
{
    auto pos = str.find_first_not_of(trim);
    if (pos != std::string::npos)
    {
        return str.substr(pos);
    }
    return std::string();
}

inline std::string TrimRight(const std::string &str, char trim = ' ')
{
    auto pos = str.find_last_not_of(trim);
    if (pos != std::string::npos)
    {
        return str.substr(0, pos + 1);
    }
    return std::string();
}

inline std::string TrimRight(const std::string &str, const char *trim)
{
    auto pos = str.find_last_not_of(trim);
    if (pos != std::string::npos)
    {
        return str.substr(0, pos + 1);
    }
    return std::string();
}

inline std::string StringUtil::Trim(const std::string &str, char trim)
{
    auto begin_pos = str.find_first_not_of(trim);
    if (begin_pos == std::string::npos)
    {
        return std::string();
    }
    auto end_pos = str.find_last_not_of(trim, begin_pos);
    if (end_pos == std::string::npos || begin_pos > end_pos)
    {
        return std::string();
    }
    return str.substr(begin_pos, end_pos + 1 - begin_pos);
}

inline std::string StringUtil::Trim(const std::string &str, const char *trim)
{
    auto begin_pos = str.find_first_not_of(trim);
    if (begin_pos == std::string::npos)
    {
        return std::string();
    }
    auto end_pos = str.find_last_not_of(trim, begin_pos);
    if (end_pos == std::string::npos || begin_pos > end_pos)
    {
        return std::string();
    }
    return str.substr(begin_pos, end_pos + 1 - begin_pos);
}

inline std::vector<std::string> Split(const std::string &str, char delimiter)
{
    std::vector<std::string> v;
    std::size_t prev_pos = 0;
    std::size_t pos = 0;
    while ((pos = str.find_first_of(delimiter, prev_pos)) != std::string::npos)
    {
        if (pos > prev_pos)
        {
            v.emplace_back(str.substr(prev_pos, pos - prev_pos));
        }
        prev_pos = pos + 1;
    }
    if (prev_pos < str.length())
    {
        v.emplace_back(str.substr(prev_pos));
    }
    return v;
}

inline std::vector<std::string> Split(const std::string &str, const char *delimiter)
{
    std::vector<std::string> v;
    std::size_t prev_pos = 0;
    std::size_t pos = 0;
    while ((pos = str.find_first_of(delimiter, prev_pos)) != std::string::npos)
    {
        if (pos > prev_pos)
        {
            v.emplace_back(str.substr(prev_pos, pos - prev_pos));
        }
        prev_pos = pos + 1;
    }
    if (prev_pos < str.length())
    {
        v.emplace_back(str.substr(prev_pos));
    }
    return v;
}

}

#endif
