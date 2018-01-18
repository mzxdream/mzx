#include <mzx/cmd_manager.h>
#include <cstdio>

namespace mzx {

CmdManager::CmdManager()
{
}

CmdManager::~CmdManager()
{
}

void CmdManager::RegistCmd(const std::string &cmd, const CmdManager::Callback &callback)
{
    callback_list_[cmd] = callback;
}

void CmdManager::UnregistCmd(const std::string &cmd)
{
    callback_list_.erase(cmd);
}

void CmdManager::ExcuteCmd()
{
    std::list<std::string> cmd_list;
    {
        std::lock_guard<std::mutex> lock(cmd_mutex_);
        cmd_list.swap(cmd_list_);
    }
    for (auto &cmd : cmd_list)
    {
        auto iter_callback = callback_list_.find(cmd);
        if (iter_callback == callback_list_.end())
        {
            continue;
        }
        if (iter_callback->second)
        {
            (iter_callback->second)();
        }
    }
}

void CmdManager::_Run()
{
    printf("cmd>");
    char cmd_buf[1024] = {0};
    while (!StopFlag())
    {
        fflush(stdout);
        char *cmd_str = fgets(cmd_buf, sizeof(cmd_buf), stdin);
        if (cmd_str)
        {
            for (int i = 0; cmd_str[i]; ++i)
            {
                if (cmd_str[i] == '\r' || cmd_str[i] == '\n')
                {
                    cmd_str[i] = '\0';
                    break;
                }
            }
            if (cmd_str[0] == '\0')
            {
                printf("cmd>");
                continue;
            }
            {
                std::lock_guard<std::mutex> lock(cmd_mutex_);
                cmd_list_.push_back(cmd_str);
            }
        }
        else if (feof(stdin))
        {
            return;
        }
    }
}

}
