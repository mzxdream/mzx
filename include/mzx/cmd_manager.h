#ifndef __MZX_CMD_MANAGER_H__
#define __MZX_CMD_MANAGER_H__

#include <map>
#include <functional>
#include <mutex>
#include <string>
#include <list>
#include <mzx/thread.h>
#include <mzx/singleton.h>

namespace mzx {

class CmdManager
    : public Thread
    , public Singleton<CmdManager>
{
    friend class Singleton<CmdManager>;
public:
    using Callback = std::function<void ()>;
private:
    CmdManager();
    virtual ~CmdManager();
    CmdManager(const CmdManager &) = delete;
    CmdManager& operator=(const CmdManager &) = delete;
public:
    void RegistCmd(const std::string &cmd, const Callback &callback);
    void UnregistCmd(const std::string &cmd);
    void ExcuteCmd();
private:
    virtual void _Run() override;
private:
    std::map<std::string, Callback> callback_list_;
    std::mutex cmd_mutex_;
    std::list<std::string> cmd_list_;
};

}

#endif
