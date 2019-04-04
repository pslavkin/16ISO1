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
   .hook      = hook3
};

void* task3(void* a)
{
   uint32_t i;
   while(1) {
      taskDelay( 5);
      gpioToggle(LED3);
      mutexLock(&semphrTask1);
         uartWriteString( UART_USB , task3Params.name);
         uartWriteString( UART_USB , "esta es la 3 y tambien es larga \r\n");
      mutexUnlock(&semphrTask1);
   }
   return NULL;
}
void* hook3(void* p) {}
