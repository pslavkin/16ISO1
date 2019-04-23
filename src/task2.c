#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "semphr.h"
#include "mutex.h"
#include "systick.h"
#include "task1.h"
#include "task2.h"
#include "taskprint.h"
#include "stat.h"

uint32_t task2Pool[REASONABLE_STACK];

taskParams_t task2Params = {
   .name      = "task2",
   .pool      = task2Pool,
   .poolSize  = sizeof(task2Pool)/sizeof(task2Pool[0]),
   .param     = NULL,
   .func      = task2,
   .hook      = defaultHook,
   .init      = NULL,
};

void* task2(void* a)
{
   float  pi=3.14;
   uint8_t data[20];                // se necesitan 15 para el numero mas grande representable +2147483647.647
   while(1) {
      semphrTake ( &task1Semphr ); // con este mutex me evito que si otra
      gpioToggle ( LED2          );
      pi=pi*1.001;
      queueWrite(&printQueue,ftostr(pi,data));
   }
   return NULL;
}
