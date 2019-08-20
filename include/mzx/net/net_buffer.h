#ifndef __MZX_NET_BUFFER_H__
#define __MZX_NET_BUFFER_H__

#include <cstdint>

namespace mzx
{

// TODO
struct NetBuffer final
{
    char *data;
    std::size_t size;
    std::size_t start_pos;
    std::size_t end_pos;
};

} // namespace mzx

#endif
