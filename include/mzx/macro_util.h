#ifndef __MZX_MACRO_UTIL_H__
#define __MZX_MACRO_UTIL_H__

namespace mzx {

template<typename ...Args>
constexpr auto VaArgsCount(Args &&...) -> decltype(sizeof...(Args))
{
    return sizeof...(Args);
}

}

#define MZX_CAT(A, B) A ## B
#define __MZX_VA_SELCT(NAME, NUM) MZX_CAT(NAME, NUM)

//#define MZX_VA_SIZE(...) (sizeof((int[]){__VA_ARGS__})/sizeof(int))
#define MZX_VA_SIZE(...) (mzx::VaArgsCount(__VA_ARGS__))
#define MZX_VA_SELECT(NAME, ...) __MZX_VA_SELCT(NAME##_, MZX_VA_SIZE(__VA_ARGS__))(__VA_ARGS__)

#endif
