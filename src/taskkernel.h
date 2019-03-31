#ifndef TASKKERNEL
#define TASKKERNEL
/*==================[inclusiones]============================================*/
/*==================[c++]====================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*==================[macros]=================================================*/
/*==================[tipos de datos declarados por el usuario]===============*/
extern taskParams taskKernelParams;
/*==================[declaraciones de datos externos]========================*/
extern taskContext    kernelContext;
extern taskContext    *runningContext;
void* taskKernel      ( void* a );
void* hookKernel      ( void* p );
/*==================[declaraciones de funciones externas]====================*/
/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif
/*==================[end of file]============================================*/
#endif
