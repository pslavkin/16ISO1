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
   uint8_t buf;
   while(1) {
      mutexLock  ( &printfMutex  );
      //stdioPrintf(UART_USB,"Tarea= %s Numero= %d\r\n",
      //      tasks.context->name,tasks.context->number);
      stdioPrintf(UART_USB,"Ingrese codigo\r\n");
      mutexUnlock ( &printfMutex );
 
      while(uartReadByte( UART_USB, &buf)==false) {
         taskDelay(mseg2Ticks(100));
         }
      switch (buf) {
         case '1':
            mutexLock  ( &printfMutex  );
            stdioPrintf(UART_USB,"operacion 1\r\n");
            semphrGive(&printfSemphr,1);
            mutexUnlock ( &printfMutex );
            break;
         case '2':
            mutexLock  ( &printfMutex  );
            stdioPrintf(UART_USB,"operacion 2\r\n");
            semphrGive(&printfSemphr,2);
            mutexUnlock ( &printfMutex );
            break;
         case '3':
            mutexLock  ( &printfMutex  );
            stdioPrintf(UART_USB,"operacion 3\r\n");
            semphrGive(&printfSemphr,3);
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

