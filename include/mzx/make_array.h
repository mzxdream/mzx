#ifndef __MZX_MAKE_ARRAY_H__
#define __MZX_MAKE_ARRAY_H__

#include <array>

namespace mzx {

template <std::size_t ...I>
struct Seq
{
    using type = Seq;
};

template <typename T1, typename T2>
struct ConcatSeq;

template <std::size_t ...I1, std::size_t ...I2>
struct ConcatSeq<Seq<I1...>, Seq<I2...>>
    : public Seq<I1..., (sizeof...(I1) + I2)...>
{

};

template <std::size_t N>
struct MakeSeq
    : public ConcatSeq<typename MakeSeq<N/2>::type, typename MakeSeq<N - N/2>::type>
{

};

template <>
struct MakeSeq<0>
    : public Seq<>
{

};

template <>
struct MakeSeq<1>
    : Seq<0>
{

};

template <class Function, std::size_t ...Indices>
constexpr auto MakeArrayHelper(Function f, Seq<Indices...>)
    -> std::array<typename std::result_of<Function(std::size_t)>::type, sizeof...(Indices)>
{
    return {{f(Indices)...}};
}

template <std::size_t N, class Function>
constexpr auto MakeArray(Function f)
    -> std::array<typename std::result_of<Function(std::size_t)>::type, N>
{
    return MakeArrayHelper(f, MakeSeq<N>{});
}

}

#endif
