#ifndef __UNICODE_UTIL_H__
#define __UNICODE_UTIL_H__

#include <functional>

namespace mzx {

static void ForeachUtf8Char(const char *utf8str, std::function<bool (const char *, std::size_t len)> cb);

}

#endif
