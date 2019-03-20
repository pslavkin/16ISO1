#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "task1.h"

taskContext    context1;

void* task1(void* a)
{
   uint32_t i;
   while(1) {
      taskDelay(3000);
      gpioToggle(LED1);
      uartWriteString( UART_USB , context1.name);
      uartWriteString( UART_USB , "\r\n");
   }
   return NULL;
}
void* hook1(void* p) {}
