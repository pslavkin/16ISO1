#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "task3.h"

taskContext    context3;

void* task3(void* a)
{
   uint32_t i;
   while(1) {
      for (i=0;i<5000000;i++)
         ;
      gpioToggle(LED3);
      uartWriteString( UART_USB , context3.name);
      uartWriteString( UART_USB , "\r\n");
   }
   return NULL;
}
void* hook3(void* p) {}
