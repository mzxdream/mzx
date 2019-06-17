#ifndef __MZX_MATH_H__
#define __MZX_MATH_H__

#include <climits>
#include <cmath>
#include <type_traits>

namespace mzx
{

template <typename T,
          typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
inline T NextPower2(T val)
{
    --val;
    for (std::size_t i = 1; i < sizeof(val) * CHAR_BIT; i <<= 1)
    {
        val |= (val >> i);
    }
    return ++val;
}

template <typename T,
          typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
inline bool IsPowerOf2(T val)
{
    return !(val & (val - 1));
}

template <typename T,
          typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
inline unsigned short Power2Index(T val)
{
    unsigned short index = 0;
    while (val >>= 1)
    {
        ++index;
    }
    return index;
}

template <typename T,
          typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
inline unsigned short NextPower2Index(T val)
{
    unsigned short index = Power2Index(val);
    if (!IsPowerOf2(val))
    {
        ++index;
    }
    return index;
}

#define MZX_ISZERO(num) (std::fpclassify(num) == FP_ZERO)
#define MZX_ISEQUAL(num1, num2) MZX_ISZERO(num1 - num2)

} // namespace mzx

#endif
