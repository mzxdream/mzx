#ifndef __UNICODE_UTIL_H__
#define __UNICODE_UTIL_H__

#include <functional>

namespace mzx {

static void ForeachUtf8Char(const char *utf8str, std::function<bool (const char *, std::size_t len)> cb);
static bool IsChineseUtf8Char(const char *str, std::size_t len);
static bool IsEnglishUtf8Char(const char *str, std::size_t len);
static bool IsNumberUtf8Char(const char *str, std::size_t len);
static bool IsAsciiUtf8Char(const char *str, std::size_t len);

}

#endif
