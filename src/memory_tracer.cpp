#include <mzx/memory_tracer.h>

namespace mzx {

MemTracer::MemoryListType MemTracer::memory_list_;
std::mutex MemTracer::mtx_;
MemTracer::ExitHandler MemTracer::exit_handler_;
MemTracerExit MemTracer::exit_;

}
