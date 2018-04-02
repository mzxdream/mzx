#include <mzx/system/sys_util.h>
#include <unistd.h>

namespace mzx {

int SysUtil::GetCPUCount()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

}
