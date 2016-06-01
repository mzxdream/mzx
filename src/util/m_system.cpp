#include <mzx/util/m_system.h>
#include <unistd.h>

long MSystem::GetCPUCount()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}
