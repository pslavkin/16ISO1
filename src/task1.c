#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "task1.h"
#include "semphr.h"


semphr_t semphrTask1;

uint32_t task1Pool[MIN_STACK];

taskParams task1Params = {
   .name      = "task1",
   .pool      = task1Pool,
   .pool_size = sizeof(task1Pool)/sizeof(task1Pool[0]),
   .param     = NULL,
   .func      = task1,
   .hook      = hook1
};

void* task1(void* a)
{
   uint32_t i,j;
   mutexInit(&semphrTask1);

   while(1) {
      //      taskDelay(100);
      for(j=0;j<10;j++) {
         for(i=0;i<1000000;i++)
            ;
         gpioToggle(LED1);
         mutexLock(&semphrTask1);
            uartWriteString( UART_USB , task1Params.name);
            uartWriteString( UART_USB , " hola este es un texto laaaaargo para que tarde un bien rato \r\n");
         mutexUnlock(&semphrTask1);
      }
      taskDelay(1);
      //taskYield();
   }
   return NULL;
}
void* hook1(void* p) {}
