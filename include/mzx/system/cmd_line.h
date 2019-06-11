#ifndef __MZX_CMD_LINE_H__
#define __MZX_CMD_LINE_H__

#include <functional>
#include <string>
#include <vector>

namespace mzx
{

class CmdLine
{
public:
    using Callback = std::function<void(const std::vector<std::string> &)>;

public:
    static bool Start();
    static void Stop();
    static void Regist(const Callback &callback);
    static void Regist(const std::string &cmd, const Callback &callback);
    static void Unregist();
    static void Unregist(const std::string &cmd);
    static void UnregistAll();
    static void Execute();
};

} // namespace mzx

#endif
