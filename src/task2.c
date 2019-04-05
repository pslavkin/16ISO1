#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "semphr.h"
#include "task1.h"
#include "task2.h"

uint32_t task2Pool[MIN_STACK];

taskParams task2Params = {
   .name      = "task2",
   .pool      = task2Pool,
   .pool_size = sizeof(task2Pool)/sizeof(task2Pool[0]),
   .param     = NULL,
   .func      = task2,
   .hook      = defaultHook,
};

void* task2(void* a)
{
   while(1) {
      taskDelay( 200);
      gpioToggle(LED2);
      mutexLock  ( &printfMutex );
         stdioPrintf(UART_USB,"Tarea= %s Numero= %d\r\n",
            tasks.context->name,tasks.context->number);
      mutexUnlock ( &printfMutex );
   }
   return NULL;
}
