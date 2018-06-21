#ifndef __DEFAULT_TYPE_H__
#define __DEFAULT_TYPE_H__

namespace mzx {

template <typename T>
class DefaultType
{
public:
    static const T Value;
};

template <typename T>
const T DefaultType<T>::Value;

}

#endif
