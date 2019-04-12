#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "semphr.h"
#include "systick.h"
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
      taskDelay(mseg2Ticks(1000));
      gpioToggle ( LED3          );
      semphrTake ( &printfSemphr ); // con este mutex me evito que si otra
      mutexLock  ( &printfMutex  );
      stdioPrintf(UART_USB,"Tarea= %s Numero= %d\r\n",
            tasks.context->name,tasks.context->number);
      stdioPrintf(UART_USB,"Ingrese codigo\r\n");
      mutexUnlock ( &printfMutex );
 
      uint8_t buf;
      while(uartReadByte( UART_USB, &buf)==false)
         ;
      switch (buf) {
         case '1':
            mutexLock  ( &printfMutex  );
            stdioPrintf(UART_USB,"operacion 1\r\n");
            mutexUnlock ( &printfMutex );
            break;
         case '2':
            mutexLock  ( &printfMutex  );
            stdioPrintf(UART_USB,"operacion 2\r\n");
            mutexUnlock ( &printfMutex );
            break;
         case '3':
            mutexLock  ( &printfMutex  );
            stdioPrintf(UART_USB,"operacion 3\r\n");
            mutexUnlock ( &printfMutex );
            break;
         default:
            mutexLock  ( &printfMutex  );
            stdioPrintf(UART_USB,"operacion invalida\r\n");
            mutexUnlock ( &printfMutex );
            break;
      }

   }
   return NULL;
}

