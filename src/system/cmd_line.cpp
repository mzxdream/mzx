#include <mzx/system/cmd_line.h>
#include <cstdio>
#include <unistd.h>

namespace mzx {
namespace system {

std::map<std::string, CmdLine::Callback> CmdLine::callback_list_;
std::mutex CmdLine::cmd_mutex_;
std::list<std::string> CmdLine::cmd_list_;
CmdLine::WorkThread CmdLine::thread_;

bool CmdLine::Start()
{
    return thread_.Start();
}

void CmdLine::Stop()
{
    thread_.StopAndJoin();
}

void CmdLine::Regist(const std::string &cmd, const CmdLine::Callback &callback)
{
    callback_list_[cmd] = callback;
}

void CmdLine::Unregist(const std::string &cmd)
{
    callback_list_.erase(cmd);
}

void CmdLine::UnregistAll()
{
    callback_list_.clear();
}

void CmdLine::Execute()
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
            (iter_callback->second)(cmd);
        }
    }
    if (!cmd_list.empty())
    {
        printf("\ncmd>");
        fflush(stdout);
    }
}

int KeyBoardHitReturn()
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

void CmdLine::WorkThread::_Run()
{
    printf("cmd>");
    char cmd_buf[1024] = {0};
    while (!StopFlag())
    {
        fflush(stdout);
        while (!KeyBoardHitReturn() && !StopFlag())
        {
            usleep(100);
        }
        if (StopFlag())
        {
            return;
        }
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
                std::lock_guard<std::mutex> lock(CmdLine::cmd_mutex_);
                CmdLine::cmd_list_.push_back(cmd_str);
            }
        }
        else if (feof(stdin))
        {
            return;
        }
    }
}

}
}
