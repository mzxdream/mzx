#include <mzx/make_array.h>
#include <iostream>

constexpr std::size_t power(std::size_t i)
{
    return (i+1) * (i+1);
}

constexpr auto arr = mzx::MakeArray<100>(power);

int main(int argc, char *argv[])
{
    for (auto &a : arr)
    {
        std::cout << a << ",";
    }
    std::cout << std::endl;
    return 0;
}
