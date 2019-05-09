#include <iostream>
#include <mzx/convert.h>

int main(int argc, char *argv[])
{
    auto a = mzx::ConvertTo<int>("123");
    std::cout << a << std::endl;
    auto b = mzx::ConvertTo<double>("123.456");
    std::cout << b << std::endl;
    // auto c = mzx::ConvertTo<int>("123.456adb");
    // std::cout << c << std::endl;
    auto d = mzx::ConvertTo("456.03", &b);
    std::cout << "b:" << b << " d:" << d << std::endl;
    d = mzx::ConvertTo("789.0a", &b);
    std::cout << "b:" << b << " d:" << d << std::endl;
    return 0;
}
