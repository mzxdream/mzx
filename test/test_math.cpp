#include <iostream>
#include <mzx/math.h>

int main(int argc, char *argv[])
{
    std::cout << "100 NextPower2:" << mzx::NextPower2((unsigned)100) << std::endl;
    std::cout << "128 NextPower2:" << mzx::NextPower2((unsigned)128) << std::endl;
    std::cout << "6 IsPowerOf2:" << mzx::IsPowerOf2((unsigned)6) << std::endl;
    std::cout << "100 IsPowerOf2:" << mzx::IsPowerOf2((unsigned)100) << std::endl;
    std::cout << "128 IsPowerOf2:" << mzx::IsPowerOf2((unsigned)128) << std::endl;
    std::cout << "100 Power2Index:" << mzx::Power2Index((unsigned)100) << std::endl;
    std::cout << "128 Power2Index:" << mzx::Power2Index((unsigned)128) << std::endl;
    std::cout << "100 NextPower2Index:" << mzx::NextPower2Index((unsigned)100) << std::endl;
    std::cout << "128 NextPower2Index:" << mzx::NextPower2Index((unsigned)128) << std::endl;
    return 0;
}
