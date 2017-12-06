#ifndef __MZX_ENUM_UTIL_H_
#define __MZX_ENUM_UTIL_H_

#include <type_traits>

namespace mzx {

template<typename E>
typename std::underlying_type<E>::type Enum2Value(E e)
{
    return static_cast<typename std::underlying_type<E>::type>(e);
}

}

#endif
