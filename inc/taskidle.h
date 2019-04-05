#ifndef TASKIDLE
#define TASKIDLE
/*==================[inclusiones]============================================*/
extern taskParams     taskIdleParams;
extern taskContext    idleContext;
/*==================[declaraciones de datos externos]========================*/
void* taskIdle      ( void* a );
void* hookIdle      ( void* p );
/*==================[end of file]============================================*/
#endif
