#include <stdint.h>
#include "sapi.h"
#include "os.h"
#include "task1.h"
#include "stat.h"
#include "systick.h"
#include "task3.h"
#include "taskmenu.h"
#include "taskprint.h"

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
   printUART ("1 - stats\r\n"
               "2 - ticks\r\n"
               "3 - uptime\r\n"
               "4 - task3Create\r\n"
               "5 - task3Delete\r\n"
               "? - help\r\n");
}
void* taskMenu(void* a)
{
   uint8_t buf;
   printUART ("\r\nPablo Slavkin - Examen iso1\r\n" );
   printHelpMenu();
   while(1) {
      printUART ("ingrese codigo>" );
      while(uartReadByte( UART_USB, &buf)==false) {
         taskDelay(msec2Ticks(100));
      }
      printUART ("\r\n" );
      switch (buf) {
         case '1':
            printTasksStat(&tasks);
            break;
         case '2':
            printUART ("ticks=%d\r\n",getTicks( ));
            break;
         case '3':
            printUpTime();
            break;
         case '4':
            if(taskFind(&task3Params)==NULL)
               taskCreate ( &task3Params,1);
            break;
         case '5':
            taskDelete (taskFind(&task3Params));
            break;
         case '?':
            printHelpMenu();
            break;
         default:
            printUART ("invalid\r\n");
            break;
      }
   }
   return NULL;
}
