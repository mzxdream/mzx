#ifndef __MZX_MACRO_UTIL_H__
#define __MZX_MACRO_UTIL_H__

#define MZX_PRIMITIVE_TOSTRING(x) #x
#define MZX_TOSTRING(x) MZX_PRIMITIVE_TOSTRING(x)

#define MZX_PRIMITIVE_CAT(x, y) x ## y
#define MZX_CAT(x, y) MZX_PRIMITIVE_CAT(x, y)

#define MZX_COMPL(x) MZX_PRIMITIVE_CAT(MZX_COMPL_, x)
#define MZX_COMPL_0 1
#define MZX_COMPL_1 0

#define MZX_IIF(x) MZX_PRIMITIVE_CAT(MZX_IIF_, x) 
#define MZX_IIF_0(x, ...) __VA_ARGS__
#define MZX_IIF_1(x, ...) x 

#define MZX_GET_SECOND(x, y, ...) y
#define MZX_CHECK(...) MZX_GET_SECOND(__VA_ARGS__, 0)
#define MZX_PROBE(x) x, 1

#define MZX_IS_EMPTY(x) MZX_CHECK(MZX_CAT(MZX_PRIMITIVE_CAT(MZX_IS_EMPTY_, x), 0))
#define MZX_IS_EMPTY_0 MZX_PROBE()

#define MZX_ARG_N( \
          _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, \
    _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, \
    _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, \
    _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, \
    _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, \
    _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, \
    _60, _61, _62, _63, N, ...) N 
#define MZX_RSEQ_N() \
    63, 62, 61, 60, \
    59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
    49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
    39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
    29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
    19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
     9,  8,  7,  6,  5,  4,  3,  2,  1,  0
#define MZX_COMMASEQ_N() \
    1,  1,  1,  1,                          \
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  \
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  \
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  \
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  \
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  \
    1,  1,  1,  1,  1,  1,  1,  1,  0,  0
#define MZX_ARG_N_HELPER(...) MZX_ARG_N(__VA_ARGS__)
#define MZX_COMMA(...) ,
#define MZX_HAS_COMMA(...) \
    MZX_ARG_N_HELPER(__VA_ARGS__, MZX_COMMASEQ_N())
#define MZX_VA_SIZE(...) \
    MZX_VA_SIZE_HELPER1( \
        MZX_HAS_COMMA(__VA_ARGS__), \
        MZX_HAS_COMMA(MZX_COMMA __VA_ARGS__ ()), \
        MZX_ARG_N_HELPER(__VA_ARGS__, MZX_RSEQ_N()))
#define MZX_VA_SIZE_HELPER1(x, y, N) MZX_VA_SIZE_HELPER2(x, y, N)
#define MZX_VA_SIZE_HELPER2(x, y, N) MZX_VA_SIZE_HELPER3_ ## x ## y(N)
#define MZX_VA_SIZE_HELPER3_01(N) 0 
#define MZX_VA_SIZE_HELPER3_00(N) 1 
#define MZX_VA_SIZE_HELPER3_11(N) N

#define MZX_VA_SELECT(NAME, ...) MZX_CAT(NAME##_, MZX_VA_SIZE(__VA_ARGS__))(__VA_ARGS__)

#endif
