#ifndef TASKKERNEL
#define TASKKERNEL
/*==================[macros]=================================================*/
//con esto elijo entre FIFO=1 o RR=0
#define SCHED_FIFO_RR    0
/*==================[declaraciones de datos externos]========================*/
extern   taskContext    kernelContext;
extern   taskParams     taskKernelParams;
void*    taskKernel      ( void* a );
void*    hookKernel      ( void* p );
/*==================[end of file]============================================*/
#endif
