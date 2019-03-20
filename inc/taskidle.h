#ifndef TASKIDLE
#define TASKIDLE
/*==================[inclusiones]============================================*/
/*==================[c++]====================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*==================[macros]=================================================*/
/*==================[tipos de datos declarados por el usuario]===============*/
extern taskContext    contextIdle;
/*==================[declaraciones de datos externos]========================*/
void* taskIdle      ( void* a );
void* hookIdle      ( void* p );
/*==================[declaraciones de funciones externas]====================*/
/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif
/*==================[end of file]============================================*/
#endif
