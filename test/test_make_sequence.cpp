#include <mzx/make_sequence.h>
#include <array>
#include <iostream>

template <


template <std::size_t N>
const auto MakeArray() -> std::array<std::size_t, N>
{
    return std::array<std::size_t, N>{mzx::MakeSequence<N>{}};
}

int main(int argc, char *argv[])
{
    auto arr = MakeArray<10>();
    for (auto &i : arr)
    {
        std::cout << i << ",";
    }
    std::cout << std::endl;
    return 0;
}
