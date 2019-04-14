#ifndef TASKKERNEL
#define TASKKERNEL

//con esto elijo entre FIFO=1 o RR=0
#define SCHED_FIFO_RR 0
//defino si llamo a las funciones de estadisticas.. si no las llamo el kernel es un poco mas
//eficaz
#define STAT_ENABLE   1

extern   taskContext_t    kernelContext;
extern   taskParams_t     taskKernelParams;

void*    taskKernel       ( void* a    );
void*    hookKernel       ( void* p    );
bool     freeBlockedTaked ( event_t* m );
bool     freeBlockedGived ( event_t* m );

#endif
