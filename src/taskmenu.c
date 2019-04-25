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

uint32_t taskMenuPool[BIG_STACK];

taskParams_t taskMenuParams = {
   .name      = "taskMenu",
   .pool      = taskMenuPool,
   .poolSize  = sizeof(taskMenuPool)/sizeof(taskMenuPool[0]),
   .param     = NULL,
   .func      = taskMenu,
   .hook      = defaultHook,
   .init      = NULL,
};

void printHelpMenu(void)
{
   uint8_t i;
   uint8_t* help[]= {
      "1 - hace 1 semphrGive\r\n",
      "2 - hace 2 semphrGive\r\n",
      "3 - hace 3 semphrGive\r\n",
      "4 - stats\r\n",
      "5 - ticks\r\n"
   };
   for(i=0;i<sizeof(help)/sizeof(help[0]);i++) {
      queueWrite(&printQueue,help[i]);
   }
}
void* taskMenu(void* a)
{
   uint8_t buf;
   while(1) {
      queueWrite  ( &printQueue,"ingrese codigo>" );
      while(uartReadByte( UART_USB, &buf)==false) {
         taskDelay(msec2Ticks(100));
      }
      queueWrite  ( &printQueue,"\r\n" );
      switch (buf) {
         case '1':
//            if(semphrGiveTout(&task1Semphr,1,msec2Ticks(100))==false)
//               queueWrite ( &printQueue,"agoto semph\r\n");
            break;
         case '2':
//            semphrGive(&task1Semphr,2);
            break;
         case '3':
         printBuffer(task1Queue.cb);
 //           semphrGive(&task1Semphr,3);
            break;
         case '4':
            printTasksStat(&tasks);
            break;
         case '5': {
            uint8_t data[5*MAX_MSG_LENGTH];
            sprintf    ( data,"ticks=%d\r\n",getTicks( ));
            queueWrite ( &printQueue,data              ) ;
            }
            break;
         case '6':
      mutexLock(&printMutex);
            uartWriteString ( UART_USB    ,"prueba de que corrompe\r\n"  );
      mutexUnlock(&printMutex);
            break;
         case '?':
            printHelpMenu();
            break;
         default:
            queueWrite ( &printQueue,"invalid\r\n");
            break;
      }
   }
   return NULL;
}

