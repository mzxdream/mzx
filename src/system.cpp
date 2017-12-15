#include <unistd.h>
#include <mzx/system.h>

namespace mzx {

int System::GetCPUCount()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

}
