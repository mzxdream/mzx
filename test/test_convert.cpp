#include <iostream>
#include <mzx/convert.h>

int main(int argc, char *argv[])
{
    int a = 0;
    auto b = mzx::TryConvertTo("456.03", &a);
    std::cout << "a:" << a << " b:" << b << std::endl;

    auto c = mzx::ConvertTo<int>("123");
    std::cout << "c:" << c << std::endl;

    auto d = mzx::ConvertTo("789.0a", 1);
    std::cout << "d:" << d << std::endl;
    return 0;
}
