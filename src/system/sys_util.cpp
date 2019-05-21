#include <cstdlib>
#include <cxxabi.h>
#include <execinfo.h>
#include <memory>
#include <unistd.h>

#include <mzx/logger.h>
#include <mzx/system/sys_util.h>

namespace mzx
{

int GetCPUCount()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

std::string Demangle(const char *name)
{
    int status = -4;
    std::unique_ptr<char, void (*)(void *)> res{abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free};
    return status == 0 ? res.get() : name;
}

void ForeachStackTrace(std::function<bool(const char *)> cb)
{
    MZX_CHECK(cb != nullptr);
    void *addresses[128];
    auto size = backtrace(addresses, sizeof(addresses) / sizeof(addresses[0]));
    std::unique_ptr<char *, void (*)(void *)> symbols{backtrace_symbols(addresses, size), std::free};
    for (decltype(size) i = 0; i < size; ++i)
    {
        auto *symbol = symbols.get()[i];
        char *mangled_begin = nullptr;
        char *mangled_end = nullptr;
        for (auto *p = symbol; *p; ++p)
        {
            if (*p == '(')
            {
                mangled_begin = p + 1;
            }
            else if (*p == '+')
            {
                mangled_end = p;
                break;
            }
        }
        if (mangled_begin && mangled_end && mangled_begin < mangled_end)
        {
            *mangled_end = '\0';
            int status = -4;
            std::unique_ptr<char, void (*)(void *)> res{abi::__cxa_demangle(mangled_begin, nullptr, nullptr, &status),
                                                        std::free};
            *mangled_end = '+';
            if (status == 0)
            {
                std::string mangled_symbol(symbol, mangled_begin - symbol - 1);
                mangled_symbol += res.get();
                mangled_symbol += mangled_end;
                if (!cb(mangled_symbol.c_str()))
                {
                    break;
                }
                continue;
            }
        }
        if (!cb(symbol))
        {
            break;
        }
    }
}

} // namespace mzx
