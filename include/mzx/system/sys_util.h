#ifndef __MZX_SYS_UTIL_H__
#define __MZX_SYS_UTIL_H__

#include <functional>

namespace mzx
{

int CPUCount();
std::string Demangle(const char *name);
std::string BackTrace();
std::string ExecName();
std::string Addr2Line(const char *exec_name, const char *addr);

} // namespace mzx

#endif
