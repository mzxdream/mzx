#ifndef __MZX_ENUM_UTIL_H__
#define __MZX_ENUM_UTIL_H__

#include <type_traits>

namespace mzx {

template<typename E>
typename std::underlying_type<E>::type EnumToValue(E e)
{
    return static_cast<typename std::underlying_type<E>::type>(e);
}

}

#endif
