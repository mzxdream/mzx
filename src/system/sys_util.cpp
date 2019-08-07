#include <cstdlib>
#include <cxxabi.h>
#include <execinfo.h>
#include <linux/limits.h>
#include <memory>
#include <sstream>
#include <unistd.h>

#include <mzx/logger.h>
#include <mzx/system/sys_util.h>

namespace mzx
{

int CPUCount()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

std::string Demangle(const char *name)
{
    int status = -4;
    std::unique_ptr<char, void (*)(void *)> res{
        abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free};
    return status == 0 ? res.get() : name;
}

std::string BackTrace()
{
    void *bt[128];
    auto bt_size = backtrace(bt, sizeof(bt) / sizeof(bt[0]));
    std::unique_ptr<char *, void (*)(void *)> bt_symbols{
        backtrace_symbols(bt, bt_size), std::free};
    std::ostringstream ss;
    ss << "backtrace begin\n";
    for (decltype(bt_size) i = 1; i < bt_size; ++i)
    {
        ss << bt[i] << " -> ";
        auto *symbol = bt_symbols.get()[i];
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
            std::unique_ptr<char, void (*)(void *)> res{
                abi::__cxa_demangle(mangled_begin, nullptr, nullptr, &status),
                std::free};
            *mangled_end = '+';
            if (status == 0)
            {
                ss.write(symbol, mangled_begin - symbol);
                ss << res.get() << mangled_end << '\n';
                continue;
            }
        }
        ss << symbol << '\n';
    }
    ss << "backtrace end\n";
    return ss.str();
}

std::string ExecName()
{
    char exec_name[PATH_MAX + 1] = {0};
    readlink("/proc/self/exe", exec_name,
             sizeof(exec_name) / sizeof(exec_name[0]) - 1);
    return exec_name;
}

std::string Addr2Line(const char *exec_name, const char *addr)
{
    if (!exec_name || !addr)
    {
        return std::string();
    }
    std::stringstream ss;
    ss << "addr2line -i -e " << exec_name << " " << addr;
    auto *pfp = popen(ss.str().c_str(), "r");
    if (!pfp)
    {
        return std::string();
    }
    char addr_info[PATH_MAX + 1] = {0};
    fgets(addr_info, sizeof(addr_info) / sizeof(addr_info[0]) - 1, pfp);
    pclose(pfp);
    return addr_info;
}

} // namespace mzx
