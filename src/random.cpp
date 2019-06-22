#include <mzx/random.h>

namespace mzx
{

std::mt19937 &RandomGenerator()
{
    static std::mt19937 gen((std::random_device())());
    return gen;
}

} // namespace mzx
