#ifndef __MZX_ENUM_UTIL_H_
#define __MZX_ENUM_UTIL_H_

#include <type_traits>

namespace mzx {

class EnumUtil
{
public:
    template<typename E>
    static typename std::underlying_type<E>::type ToValue(E e)
    {
        return static_cast<typename std::underlying_type<E>::type>(e);
    }
};

}

#endif
