#ifndef __MZX_RANDOM_H__
#define __MZX_RANDOM_H__

#include <random>

#include <mzx/logger.h>

namespace mzx
{

std::mt19937 &RandomGenerator();

inline float Random(float a, float b)
{
    return (std::uniform_real_distribution<float>(a, b))(RandomGenerator());
}

inline double Random(double a, double b)
{
    return (std::uniform_real_distribution<double>(a, b))(RandomGenerator());
}

template <typename T>
inline T Random(T a, T b)
{
    MZX_CHECK_STATIC(std::is_integral<T>::value, "Integral required.");
    MZX_CHECK(a != b);
    if (a < b)
    {
        return (std::uniform_int_distribution<T>(a, b - 1))(RandomGenerator());
    }
    return (std::uniform_int_distribution<T>(a - 1, b))(RandomGenerator());
}

} // namespace mzx

#endif
