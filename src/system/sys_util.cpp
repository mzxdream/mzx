#include <unistd.h>
#include <mzx/system/sys_util.h>

namespace mzx {
namespace system {

int System::GetCPUCount()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

}
}
