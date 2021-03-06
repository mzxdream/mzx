#include <array>
#include <iostream>

#include <mzx/data_structure/make_sequence.h>

template <std::size_t... I>
const auto MakeArray(mzx::Sequence<I...>)
    -> std::array<std::size_t, sizeof...(I)>
{
    return std::array<std::size_t, sizeof...(I)>{(I + 1)...};
}

int main(int argc, char *argv[])
{
    auto arr = MakeArray(mzx::MakeSequence<10>{});
    for (auto &i : arr)
    {
        std::cout << i << ",";
    }
    std::cout << std::endl;
    return 0;
}
