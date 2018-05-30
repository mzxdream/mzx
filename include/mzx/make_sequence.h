#ifndef __MZX_MAKE_SEQUENCE_H__
#define __MZX_MAKE_SEQUENCE_H__

namespace mzx {

template <std::size_t ...I>
struct Sequence
{
    using type = Sequence;
};

template <typename T1, typename T2>
struct ConcatSequence;

template <std::size_t ...I1, std::size_t ...I2>
struct ConcatSequence<Sequence<I1...>, Sequence<I2...>>
    : public Sequence<I1..., (sizeof...(I1) + I2)...>
{

};

template <std::size_t N>
struct MakeSequence
    : public ConcatSequence<typename MakeSequence<N/2>::type, typename MakeSequence<N - N/2>::type>
{

};

template <>
struct MakeSequence<0>
    : public Sequence<>
{

};

template <>
struct MakeSequence<1>
    : Sequence<0>
{

};

}

#endif
