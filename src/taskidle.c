#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "taskidle.h"

taskContext    contextIdle;

void* taskIdle(void* a)
{
   uint32_t i;
   while(1) {
      gpioToggle(LEDB);
      //uartWriteString( UART_USB , contextIdle.name);
      //uartWriteString( UART_USB , "\r\n");
   }
   return NULL;
}
void* hookIdle(void* p) {}
