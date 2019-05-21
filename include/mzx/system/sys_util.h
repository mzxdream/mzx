#ifndef __MZX_SYS_UTIL_H__
#define __MZX_SYS_UTIL_H__

#include <functional>
#include <string>

namespace mzx
{

int GetCPUCount();
std::string Demangle(const char *name);
void ForeachStackTrace(std::function<bool(const char *)> cb);

} // namespace mzx

#endif
