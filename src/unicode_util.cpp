#include <mzx/unicode_util.h>

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

}
