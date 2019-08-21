#ifndef __MZX_NET_WORKER_H__
#define __MZX_NET_WORKER_H__

#include <mzx/spsc_queue.h>
#include <mzx/thread.h>

namespace mzx
{

class NetWorker final
{
public:
    NetWorker();
    ~NetWorker();

public:
    bool Start();
    void Stop();

private:
    void Run();

private:
    SPSCQueue<int> input_list_;
    SPSCQueue<int> output_list_;
};

} // namespace mzx

#endif
