#ifndef __UNICODE_UTIL_H__
#define __UNICODE_UTIL_H__

#include <functional>

namespace mzx {

void ForeachUtf8Char(const char *utf8str, std::function<bool (const char *, std::size_t len)> cb);
bool IsChineseUtf8Char(const char *str, std::size_t len);
bool IsEnglishUtf8Char(const char *str, std::size_t len);
bool IsNumberUtf8Char(const char *str, std::size_t len);
bool IsAsciiUtf8Char(const char *str, std::size_t len);

}

#endif
