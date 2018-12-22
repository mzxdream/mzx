#include <mzx/string_util.h>

namespace mzx {

std::string StringUtil::TrimLeft(const std::string &str, const std::string &trim)
{
    auto pos = str.find_first_not_of(trim);
    if (pos != std::string::npos)
    {
        return str.substr(pos);
    }
    return std::string();
}

std::string StringUtil::TrimRight(const std::string &str, const std::string &trim)
{
    auto pos = str.find_last_not_of(trim);
    if (pos != std::string::npos)
    {
        return str.substr(0, pos + 1);
    }
    return std::string();
}

std::string StringUtil::Trim(const std::string &str, const std::string &trim)
{
    auto begin_pos = str.find_first_not_of(trim);
    if (begin_pos == std::string::npos)
    {
        return std::string();
    }
    auto end_pos = str.find_last_not_of(trim);
    if (end_pos == std::string::npos || begin_pos > end_pos)
    {
        return std::string();
    }
    return str.substr(begin_pos, end_pos + 1 - begin_pos);
}

std::vector<std::string> StringUtil::Split(const std::string &str, char delimiter)
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

std::vector<std::string> StringUtil::Split(const std::string &str, const std::string &delimiter)
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
