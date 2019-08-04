#ifndef __MZX_NET_WORKER_H__
#define __MZX_NET_WORKER_H__

#include <mzx/thread.h>

namespace mzx
{

class NetWorker final
{
public:
    NetWorker();
    ~NetWorker();

public:
    bool Init();
    void Uninit();

private:
};

} // namespace mzx

#endif
