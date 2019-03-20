#ifndef OS
#define OS

#include <stdint.h>
#include <stdbool.h>
/*==================[inclusiones]============================================*/
/*==================[c++]====================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*==================[macros]=================================================*/
/*==================[tipos de datos declarados por el usuario]===============*/
#define MIN_STACK        100
#define TASK_NAME_LENGTH 16

typedef struct taskContext_t {
   uint32_t*   pool_index;
   uint32_t    pool[MIN_STACK];
   char        name[TASK_NAME_LENGTH];
   uint32_t    state;
} taskContext;
extern taskContext    kernelContext;
/*==================[declaraciones de datos externos]========================*/
bool taskCreate   ( const char* name, void* (*func )(void*),void* param, taskContext* context, void* (*hook)(void*));
bool kernelCreate ( const char* name, void* (*func )(void*),void* param, taskContext* context, void* (*hook)(void*));
void* kernelTask ( void* p );
void* hookKernel ( void* p );
/*==================[declaraciones de funciones externas]====================*/
/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif
/*==================[end of file]============================================*/
#endif
