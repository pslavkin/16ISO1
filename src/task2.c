#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "task2.h"

taskContext    context2;

void* task2(void* a)
{
   uint32_t i;
   while(1) {
      for (i=0;i<4000000;i++)
         ;
      gpioToggle(LED2);
      uartWriteString( UART_USB , context2.name);
      uartWriteString( UART_USB , "\r\n");
   }
   return NULL;
}
void* hook2(void* p) {}
