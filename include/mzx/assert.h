#ifndef __MZX_ASSERT_H__
#define __MZX_ASSERT_H__

#include <cassert>
#include <string>

namespace mzx {

inline void Assert(bool condition)
{
    assert(condition);
}

inline void Assert(bool condition, const std::string &message)
{
    if (!condition)
    {
        std::cerr << message << std::endl;
    }
    Assert(condition);
}

}

#endif
