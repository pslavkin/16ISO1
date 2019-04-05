#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "semphr.h"
#include "task1.h"
#include "task3.h"

uint32_t task3Pool[MIN_STACK];

taskParams task3Params = {
   .name      = "task3",
   .pool      = task3Pool,
   .pool_size = sizeof(task3Pool)/sizeof(task3Pool[0]),
   .param     = NULL,
   .func      = task3,
   .hook      = defaultHook,
};

void* task3(void* a)
{
   while(1) {
      taskDelay( 3);
      gpioToggle(LED3);
      mutexLock  ( &printfMutex );
         stdioPrintf(UART_USB,"Tarea= %s Numero= %d\r\n",
            tasks.context->name,tasks.context->number);
      mutexUnlock ( &printfMutex );
   }
   return NULL;
}

