#ifndef __MZX_SYS_UTIL_H__
#define __MZX_SYS_UTIL_H__

#include <functional>

namespace mzx
{

int CPUCount();
std::string Demangle(const char *name);
std::string BackTrace();

} // namespace mzx

#endif
