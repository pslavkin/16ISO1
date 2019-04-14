#ifndef TASKKERNEL
#define TASKKERNEL

//con esto elijo entre FIFO=1 o RR=0
#define SCHED_FIFO_RR    0

extern   taskContext    kernelContext;
extern   taskParams     taskKernelParams;

void*    taskKernel       ( void* a    );
void*    hookKernel       ( void* p    );
bool     freeBlockedTaked ( event_t* m );
bool     freeBlockedGived ( event_t* m );

#endif
