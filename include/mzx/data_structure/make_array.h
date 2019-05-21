#ifndef __MZX_MAKE_ARRAY_H__
#define __MZX_MAKE_ARRAY_H__

#include <array>

#include <mzx/data_structure/make_sequence.h>

namespace mzx
{

template <class Function, std::size_t... Indices>
constexpr auto MakeArrayHelper(Function f, Sequence<Indices...>)
    -> std::array<typename std::result_of<Function(std::size_t)>::type, sizeof...(Indices)>
{
    return {{f(Indices)...}};
}

template <std::size_t N, class Function>
constexpr auto MakeArray(Function f) -> std::array<typename std::result_of<Function(std::size_t)>::type, N>
{
    return MakeArrayHelper(f, MakeSequence<N>{});
}

} // namespace mzx

#endif
