#include <mzx/macro_util.h>
#include <iostream>

#define TEST_0() std::cout << "TEST_0()" << std::endl;
#define TEST_1(x) std::cout << "TEST_1(" << x << ")" << std::endl;
#define TEST_2(x, y) std::cout << "TEST_2(" << x << "," << y << ")" << std::endl;
#define TEST_3(x, y, z) std::cout << "TEST_3(" << x << "," << y << "," << z << ")" << std::endl;
#define TEST(...) MZX_VA_SELECT(TEST, __VA_ARGS__)

int main(int argc, char *argv[])
{
    std::cout << MZX_VA_SIZE() << std::endl;
    std::cout << MZX_VA_SIZE(1) << std::endl;
    std::cout << MZX_VA_SIZE(1, 2) << std::endl;
    std::cout << MZX_VA_SIZE(1, 2, 3) << std::endl;
    TEST();
    TEST(1);
    TEST(1, 2);
    TEST(1, 2, 3);
    return 0;
}
