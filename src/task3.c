#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
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
      taskDelay(9000);
      gpioToggle(LED3);
      uartWriteString( UART_USB , task3Params.name);
      uartWriteString( UART_USB , "\r\n");
   }
   return NULL;
}
void* hook3(void* p) {}
