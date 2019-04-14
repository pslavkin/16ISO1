#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "mutex.h"
#include "semphr.h"
#include "systick.h"
#include "task1.h"
#include "taskmenu.h"
#include "taskprint.h"
#include "stat.h"

uint32_t taskMenuPool[MIN_STACK];

taskParams_t taskMenuParams = {
   .name      = "taskMenu",
   .pool      = taskMenuPool,
   .pool_size = sizeof(taskMenuPool)/sizeof(taskMenuPool[0]),
   .param     = NULL,
   .func      = taskMenu,
   .hook      = defaultHook,
};

void* taskMenu(void* a)
{
   uint8_t buf;
   while(1) {
      queueWrite  ( &printQueue,">" );
      while(uartReadByte( UART_USB, &buf)==false) {
         taskDelay(mseg2Ticks(100));
      }
      queueWrite  ( &printQueue,"\r\n" );
      switch (buf) {
         case '1':
            semphrGive(&printfSemphr,1);
            break;
         case '2':
            semphrGive(&printfSemphr,2);
            break;
         case '3':
            semphrGive(&printfSemphr,3);
            break;
         case '4':
            printTasksStat(&tasks);
            break;
         case '5': {
            uint8_t data[MAX_MSG_LENGTH];
            sprintf(data,"op3 tick=%d\r\n",getTicks());
            queueWrite  ( &printQueue,data );
            }
            break;
         default:
            break;
      }
   }
   return NULL;
}

