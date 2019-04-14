#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "mutex.h"
#include "semphr.h"
#include "systick.h"
#include "task1.h"
#include "task3.h"
#include "taskprint.h"

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
         stdioPrintf(UART_USB,"Ingrese codigo\r\n");
      mutexUnlock ( &printfMutex );

      while(uartReadByte( UART_USB, &buf)==false) {
         taskDelay(mseg2Ticks(100));
         }
      switch (buf) {
         case '1':
//            mutexLock  ( &printfMutex  );
            stdioPrintf(UART_USB,"operacion 1\r\n");
 //           mutexUnlock ( &printfMutex );
            semphrGive(&printfSemphr,1);
            break;
         case '2':
            //mutexLock  ( &printfMutex  );
            //stdioPrintf(UART_USB,"operacion 2\r\n");
            //mutexUnlock ( &printfMutex );
            semphrGive(&printfSemphr,2);
            break;
         case '3': {
            uint8_t data[MAX_MSG_LENGTH];
            sprintf(data,"op3 tick=%d\r\n",getTicks());
            queueWrite  ( &printQueue,data );
            }
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

