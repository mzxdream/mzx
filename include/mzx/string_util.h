#ifndef __MZX_STRING_UTIL_H__
#define __MZX_STRING_UTIL_H__

#include <string>
#include <sstream>
#include <vector>

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
    static std::string TrimLeft(const std::string &str, const std::string &trim);
    static std::string TrimRight(const std::string &str, const std::string &trim);
    static std::string Trim(const std::string &str, const std::string &trim);
    static std::vector<std::string> Split(const std::string &str, char delimiter);
    static std::vector<std::string> Split(const std::string &str, const std::string &delimiter);
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
