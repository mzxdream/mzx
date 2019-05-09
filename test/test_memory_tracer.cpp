#include <cstdio>
#include <mzx/memory_tracer.h>
#include <vector>

static void DumpMemory()
{
    fprintf(stderr, "dump memory begin\n");
    mzx::MemTracer::Dump([](const mzx::MemTracerInfo &info) {
        fprintf(stderr, "[%s:%d] %p->%zu \n", info.file, info.line, info.memory, info.size);
    });
    fprintf(stderr, "dump memory end\n");
    fflush(stderr);
}

int main(int argc, char *argv[])
{
    DumpMemory();
    char *p = new char[10];
    DumpMemory();
    delete[] p;
    DumpMemory();
    int *c = (int *)malloc(sizeof(int));
    mzx::MemTracer::SetExitHandler([]() {
        fprintf(stderr, "exit dump: \n");
        mzx::MemTracer::Dump([](const mzx::MemTracerInfo &info) {
            fprintf(stderr, "[%s:%d] %p->%zu \n", info.file, info.line, info.memory, info.size);
        });
        fprintf(stderr, "exit dump end \n");
        fflush(stderr);
    });
    std::map<int, int *> a;
    a[0] = new int(10);
    std::vector<int> b;
    DumpMemory();
    return 0;
}
