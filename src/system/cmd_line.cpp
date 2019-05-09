#include <cstdio>
#include <list>
#include <map>
#include <mutex>
#include <mzx/system/cmd_line.h>
#include <mzx/thread.h>
#include <unistd.h>

namespace mzx
{

class CmdLineWorkThread : public Thread
{
public:
    virtual void _Run();
};

static std::map<std::string, CmdLine::Callback> cmd_callback_list;
static CmdLine::Callback cmd_callback_default;
static std::mutex cmd_mutex;
static std::list<std::string> cmd_list;
static CmdLineWorkThread cmd_thread;

bool CmdLine::Start()
{
    return cmd_thread.Start();
}

void CmdLine::Stop()
{
    cmd_thread.StopAndJoin();
}

void CmdLine::Regist(const CmdLine::Callback &callback)
{
    cmd_callback_default = callback;
}

void CmdLine::Regist(const std::string &cmd, const CmdLine::Callback &callback)
{
    cmd_callback_list[cmd] = callback;
}

void CmdLine::Unregist()
{
    cmd_callback_default = nullptr;
}

void CmdLine::Unregist(const std::string &cmd)
{
    cmd_callback_list.erase(cmd);
}

void CmdLine::UnregistAll()
{
    cmd_callback_list.clear();
    cmd_callback_default = nullptr;
}

void CmdLine::Execute()
{
    std::list<std::string> cmd_execute_list;
    {
        std::lock_guard<std::mutex> lock(cmd_mutex);
        cmd_execute_list.swap(cmd_list);
    }
    for (auto &cmd : cmd_execute_list)
    {
        auto iter_callback = cmd_callback_list.find(cmd);
        if (iter_callback != cmd_callback_list.end())
        {
            if (iter_callback->second)
            {
                (iter_callback->second)(cmd);
                continue;
            }
        }
        if (cmd_callback_default)
        {
            cmd_callback_default(cmd);
        }
    }
    if (!cmd_execute_list.empty())
    {
        printf("\ncmd>");
        fflush(stdout);
    }
}

static int KeyBoardHitReturn()
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

void CmdLineWorkThread::_Run()
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
                std::lock_guard<std::mutex> lock(cmd_mutex);
                cmd_list.push_back(cmd_str);
            }
        }
        else if (feof(stdin))
        {
            return;
        }
    }
}

} // namespace mzx
