#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

void main()
{
    unsigned long long start, end, pid;

    start = __rdtsc();
    pid = getpid();
    end = __rdtsc();

    printf("Process ID : %lld\n", pid);
    printf("Time taken to run getpid() : %lld\n", (end - start));
}
