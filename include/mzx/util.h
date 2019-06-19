#ifndef __MZX_UTIL_H__
#define __MZX_UTIL_H__

#include <string>
#include <type_traits>

namespace mzx
{

template <typename E>
inline typename std::underlying_type<E>::type EnumToValue(E e)
{
    return static_cast<typename std::underlying_type<E>::type>(e);
}

struct PairHash
{
public:
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const
    {
        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
    }
};

} // namespace mzx

#endif
