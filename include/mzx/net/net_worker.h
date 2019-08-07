#ifndef __MZX_NET_WORKER_H__
#define __MZX_NET_WORKER_H__

#include <mzx/srsw_queue.h>
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
    SRSWQueue<int> input_list_;
    SRSWQueue<int> output_list_;
};

} // namespace mzx

#endif
