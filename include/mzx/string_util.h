#ifndef __MZX_STRING_UTIL_H__
#define __MZX_STRING_UTIL_H__

#include <string>
#include <sstream>

namespace mzx {

class StringUtil
{
public:
    template <typename ...Args>
    static std::string Concat(const Args& ...args)
    {
        std::stringstream ss;
        ConcatBase(ss, args...);
        return ss.str();
    }

    static std::string TrimLeft(const std::string &str, const std::string &trim)
    {
        std::size_t pos = str.find_first_not_of(trim);
        if (pos != std::string::npos)
        {
            return str.substr(pos);
        }
        return "";
    }

    static std::string TrimRight(const std::string &str, const std::string &trim)
    {
        std::size_t pos = str.find_last_not_of(trim);
        if (pos != std::string::npos)
        {
            return str.substr(0, pos + 1);
        }
        return "";
    }

    static std::string Trim(const std::string &str, const std::string &trim)
    {
        std::size_t begin_pos = str.find_first_not_of(trim);
        if (begin_pos == std::string::npos)
        {
            return "";
        }
        std::size_t end_pos = str.find_last_not_of(trim);
        if (end_pos == std::string::npos
            || begin_pos >= end_pos)
        {
            return "";
        }
        return str.substr(begin_pos, end_pos + 1);
    }
private:
    template <typename O>
    static void ConcatBase(O &os)
    {
    }

    template <typename O, typename T, typename ...Args>
    static void ConcatBase(O &os, const T &val, const Args& ...args)
    {
        os << val;
        ConcatBase(os, args...);
    }
};

}

#endif
