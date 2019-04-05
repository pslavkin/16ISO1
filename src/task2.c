#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "semphr.h"
#include "task2.h"
#include "task1.h"

uint32_t task2Pool[MIN_STACK];

taskParams task2Params = {
   .name      = "task2",
   .pool      = task2Pool,
   .pool_size = sizeof(task2Pool)/sizeof(task2Pool[0]),
   .param     = NULL,
   .func      = task2,
   .hook      = hook2
};

void* task2(void* a)
{
   uint32_t i;
   while(1) {
      taskDelay(  200);
      gpioToggle(LED2);
      mutexLock(&semphrTask1);
         stdioPrintf(UART_USB,"nombre= %s numero= %d\r\n",tasks.context->name,tasks.context->number);
      mutexUnlock(&semphrTask1);
   }
   return NULL;
}
void* hook2(void* p) {}
