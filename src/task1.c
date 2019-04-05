#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "semphr.h"
#include "task1.h"

//------------------------------------------
semphr_t printfMutex;

void printfMutexInit(void)
{
   mutexInit(&printfMutex);
}
//------------------------------------------
uint32_t task1Pool[MIN_STACK];

taskParams task1Params = {
   .name      = "task1",
   .pool      = task1Pool,
   .pool_size = sizeof(task1Pool)/sizeof(task1Pool[0]),
   .param     = NULL,
   .func      = task1,
   .hook      = defaultHook,
};


void* task1(void* a)
{
   uint32_t i,j;

   while(1) {
      for(j=0;j<100;j++) {
         for(i=0;i<1000000;i++)
            ;
         gpioToggle ( LED1         );
         mutexLock  ( &printfMutex );
            stdioPrintf(UART_USB,"Tarea= %s Numero= %d\r\n",
                  tasks.context->name,tasks.context->number);
         mutexUnlock ( &printfMutex );
      }
      taskDelay(100);
   }
   return NULL;
}
