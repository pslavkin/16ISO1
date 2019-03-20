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
#define KERNEL_STACK     20
#define TASK_NAME_LENGTH 16


enum taskState{
   ACTIVE=0,
   SLEEPING,
};

typedef struct    taskContext_t {
   uint32_t*      pool_index;
   uint32_t       pool[MIN_STACK];
   char           name[TASK_NAME_LENGTH];
   enum taskState state;
   uint32_t       sleepTicks;
   uint8_t        prior;
} taskContext;

extern taskContext    kernelContext;
/*==================[declaraciones de datos externos]========================*/
bool taskCreate(const char* name, void* (*func)(void*),void* param, taskContext* context, uint32_t prior, void* (*hook)(void*));
bool kernelCreate(const char* name, void* (*func)(void*),void* param, taskContext* context, uint32_t prior, void* (*hook)(void*));
void* kernelTask ( void* p );
void* hookKernel ( void* p );
bool taskDelay(uint32_t t)  ;
/*==================[declaraciones de funciones externas]====================*/
/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif
/*==================[end of file]============================================*/
#endif
