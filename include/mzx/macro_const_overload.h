#ifndef __MZX_MACRO_CONST_OVERLOAD_H__
#define __MZX_MACRO_CONST_OVERLOAD_H__

#include <mzx/macro_util.h>

#define MZX_NON_CONST_OVERLOAD_1(F)                                            \
    auto F()                                                                   \
    {                                                                          \
        const auto *tmp = this;                                                \
        return const_cast<                                                     \
            typename std::remove_const<decltype(tmp->F())>::type>(tmp->F());   \
    }
#define MZX_NON_CONST_OVERLOAD_3(F, T1, t1)                                    \
    auto F(T1 t1)                                                              \
    {                                                                          \
        const auto *tmp = this;                                                \
        return const_cast<                                                     \
            typename std::remove_const<decltype(tmp->F(t1))>::type>(           \
            tmp->F(t1));                                                       \
    }
#define MZX_NON_CONST_OVERLOAD_5(F, T1, t1, T2, t2)                            \
    auto F(T1 t1, T2 t2)                                                       \
    {                                                                          \
        const auto *tmp = this;                                                \
        return const_cast<                                                     \
            typename std::remove_const<decltype(tmp->F(t1, t2))>::type>(       \
            tmp->F(t1, t2));                                                   \
    }

#define MZX_NON_CONST_OVERLOAD(...)                                            \
    MZX_VA_SELECT(MZX_NON_CONST_OVERLOAD, __VA_ARGS__)

#endif
