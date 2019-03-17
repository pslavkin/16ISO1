#ifndef MAIN
#define MAIN

#include <stdint.h>
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
} taskContext;
/*==================[declaraciones de datos externos]========================*/
void* task1      ( void* a );
void* task2      ( void* a );
void* task3      ( void* a );
void* kernelTask ( void* p );
void* hookKernel(void* p);

/*==================[declaraciones de funciones externas]====================*/

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif
