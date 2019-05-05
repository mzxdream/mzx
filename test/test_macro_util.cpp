#include <mzx/macro_util.h>
#include <iostream>


#define PP_ARG_N( \
    _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
    _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
    _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
    _61, _62, _63, N, ...) N 
#define PP_RSEQ_N()                                   \
    63, 62, 61, 60,                                   \
    59, 58, 57, 56, 55, 54, 53, 52, 51, 50,           \
    49, 48, 47, 46, 45, 44, 43, 42, 41, 40,           \
    39, 38, 37, 36, 35, 34, 33, 32, 31, 30,           \
    29, 28, 27, 26, 25, 24, 23, 22, 21, 20,           \
    19, 18, 17, 16, 15, 14, 13, 12, 11, 10,           \
    9,  8,  7,  6,  5,  4,  3,  2,  1,  0
#define PP_NARG_(...)    PP_ARG_N(__VA_ARGS__)

#define PP_COMMASEQ_N()                                    \
    1,  1,  1,  1,                                   \
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
    1,  1,  1,  1,  1,  1,  1,  1,  0,  0
#define PP_COMMA(...)    , 
#define PP_HASCOMMA(...)                                   \
    PP_NARG_(__VA_ARGS__, PP_COMMASEQ_N()) 
#define PP_NARG(...)                                       \
    PP_NARG_HELPER1(                                 \
            PP_HASCOMMA(__VA_ARGS__),                    \
            PP_HASCOMMA(PP_COMMA __VA_ARGS__ ()),        \
            PP_NARG_(__VA_ARGS__, PP_RSEQ_N()))
#define PP_NARG_HELPER1(a, b, N)    PP_NARG_HELPER2(a, b, N) 
#define PP_NARG_HELPER2(a, b, N)    PP_NARG_HELPER3_ ## a ## b(N) 
#define PP_NARG_HELPER3_01(N)    0 
#define PP_NARG_HELPER3_00(N)    1 
#define PP_NARG_HELPER3_11(N)    N

#define PP_ISZERO(x)    PP_HASCOMMA(PP_ISZERO_HELPER_ ## x) 
#define PP_ISZERO_HELPER_0    ,

#define PP_PREPEND_NARG(...)                               \
    PP_PREPEND_NARG_HELPER1(PP_NARG(__VA_ARGS__), __VA_ARGS__) 
#define PP_PREPEND_NARG_HELPER1(N, ...)                    \
    PP_PREPEND_NARG_HELPER2(PP_ISZERO(N), N, __VA_ARGS__) 
#define PP_PREPEND_NARG_HELPER2(z, N, ...)                 \
    PP_PREPEND_NARG_HELPER3(z, N, __VA_ARGS__)
#define PP_PREPEND_NARG_HELPER3(z, N, ...)                 \
    PP_PREPEND_NARG_HELPER4_ ## z (N, __VA_ARGS__) 
#define PP_PREPEND_NARG_HELPER4_1(N, ...)  0 
#define PP_PREPEND_NARG_HELPER4_0(N, ...)  N, __VA_ARGS__



#define CAT( A, B ) A ## B 
#define SELECT( NAME, NUM ) CAT( NAME, NUM )
#define GET_COUNT( _1, _2, _3, _4, _5, _6 /* ad nauseam */, COUNT, ... ) COUNT
#define VA_SIZE(...) GET_COUNT(__VA_ARGS__, 6, 5, 4, 3, 2, 1 ) 
#define VA_SELECT(NAME, ...) SELECT(NAME##_, PP_NARG(__VA_ARGS__))(__VA_ARGS__)

#define TEST_0() std::cout << "TEST_0()" << std::endl;
#define TEST_1(x) std::cout << "TEST_1(" << x << ")" << std::endl;
#define TEST_2(x, y) std::cout << "TEST_2(" << x << "," << y << ")" << std::endl;
#define TEST_3(x, y, z) std::cout << "TEST_3(" << x << "," << y << "," << z << ")" << std::endl;
#define TEST(...) VA_SELECT(TEST, __VA_ARGS__)

int main(int argc, char *argv[])
{
    std::cout << MZX_VA_SIZE() << std::endl;
    std::cout << MZX_VA_SIZE(1) << std::endl;
    std::cout << MZX_VA_SIZE(1, 2) << std::endl;
    std::cout << MZX_VA_SIZE(1, 2, 3) << std::endl;
    TEST();
    TEST(1);
    TEST(1, 2);
    TEST(1, 2, 3);
    return 0;
}
