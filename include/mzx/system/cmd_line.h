#ifndef __MZX_SYSTEM_CMD_LINE_H__
#define __MZX_SYSTEM_CMD_LINE_H__

#include <map>
#include <functional>
#include <mutex>
#include <string>
#include <list>
#include <mzx/thread.h>

namespace mzx {
namespace system {

class CmdLine
{
public:
    using Callback = std::function<void (const std::string &)>;
public:
    static bool Start();
    static void Stop();
    static void Regist(const std::string &cmd, const Callback &callback);
    static void Unregist(const std::string &cmd);
    static void Excute();
private:
    class WorkThread
        : public Thread
    {
    public:
        virtual void _Run();
    };
private:
    static std::map<std::string, Callback> callback_list_;
    static std::mutex cmd_mutex_;
    static std::list<std::string> cmd_list_;
    static WorkThread thread_;
};

}
}

#endif
