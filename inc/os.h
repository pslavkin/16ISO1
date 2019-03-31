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
#define MAX_TASK         10
#define MIN_STACK        100
#define TASK_NAME_LENGTH 16

enum taskState{
   ACTIVE=0,
   SLEEPING,
   EMPTY,
};

typedef struct    taskContext_t {
   uint32_t*      sp;
   uint32_t*      pool;
   enum taskState state;
   uint32_t       sleepTicks;
   char           name[TASK_NAME_LENGTH];
   uint8_t        prior;
} taskContext;

typedef struct    taskParams_t {
   char           name[TASK_NAME_LENGTH];
   uint32_t*      pool;
   uint32_t       pool_size;
   void*          (*func)(void*);
   void*          param;
   void*          (*hook)(void*);
} taskParams;

extern taskContext    kernelContext;
extern taskParams     taskKernelParams;
extern taskContext    taskList[MAX_TASK]; //nop... esto no TODO
/*==================[declaraciones de datos externos]========================*/
bool initTasks       ( void                          );
bool taskCreate      ( taskParams* t, uint32_t prior );
void yield           ( void                          );
bool taskDelay       ( uint32_t t                    );
void Init_SysTick    ( void                          );
void SysTick_Handler ( void                          );
/*==================[declaraciones de funciones externas]====================*/
/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif
/*==================[end of file]============================================*/
#endif
