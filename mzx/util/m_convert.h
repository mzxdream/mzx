#ifndef _M_CONVERT_H_
#define _M_CONVERT_H_

#include <sstream>
#include <string>

template<typename T, typename O>
inline bool MConvertTo(const T &from, O &to)
{
    std::stringstream ss;
    return (ss << from) && (ss >> to);
}

#endif
