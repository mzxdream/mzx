#include <mzx/unicode_util.h>
#include <cstdint>

namespace mzx {

void ForeachUtf8Char(const char *utf8str, std::function<bool (const char *, std::size_t len)> cb)
{
    if (!utf8str || !cb)
    {
        return;
    }
    std::size_t prev_pos = 0;
    std::size_t pos = 0;
    while (utf8str[pos] != '\0')
    {
        auto c = static_cast<unsigned char>(utf8str[pos]);
        if (c <= 0x7f || c >= 0xc0)
        {
            if (prev_pos != pos)
            {
                if (!cb(utf8str + prev_pos, pos - prev_pos))
                {
                    return;
                }
                prev_pos = pos;
            }
        }
        ++pos;
    }
    if (prev_pos != pos)
    {
        cb(utf8str + prev_pos, pos - prev_pos);
    }
}

static uint64_t CalculateUtf8Value(const char *str, std::size_t len)
{
    if (!str || len == 0)
    {
        return 0;
    }
    const char *p = str;
    uint64_t val = 0;
    while (p != str + len)
    {
        val <<= 8;
        val += static_cast<unsigned char>(*p);
        ++p;
    }
    return val;
}

bool IsChineseUtf8Char(const char *str, std::size_t len)
{
    if (!str || len == 0)
    {
        return false;
    }
    auto val = CalculateUtf8Value(str, len);
    if (val >= 0xE4B880)
    {

    }
    if (len == 3)// ~ 0xE9BEA5
    {
    }
    return false;
}

}
