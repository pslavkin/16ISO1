#ifndef TASKIDLE
#define TASKIDLE
/*==================[inclusiones]============================================*/
extern taskParams_t   taskIdleParams;
extern taskContext_t  idleContext;
/*==================[declaraciones de datos externos]========================*/
void* taskIdle      ( void* a );
void* hookIdle      ( void* p );
/*==================[end of file]============================================*/
#endif
