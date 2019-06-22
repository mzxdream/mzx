#include <cmath>
#include <iostream>

#include <mzx/random.h>

int main(int argc, char *argv[])
{
    std::cout << std::endl;
    for (int i = 1; i <= 10; ++i)
    {
        std::cout << "(" << -i << "," << i << ")->";
        std::cout << mzx::Random(-i, i) << ";";
    }
    std::cout << std::endl;
    for (int i = 1; i <= 10; ++i)
    {
        double c = sqrt(i);
        std::cout << "(" << -c << "," << c << ")->";
        std::cout << mzx::Random(c, -c) << ";";
    }
    std::cout << std::endl;
    return 0;
}
