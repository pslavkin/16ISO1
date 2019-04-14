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

uint32_t task2Pool[MIN_STACK];

taskParams_t task2Params = {
   .name      = "task2",
   .pool      = task2Pool,
   .pool_size = sizeof(task2Pool)/sizeof(task2Pool[0]),
   .param     = NULL,
   .func      = task2,
   .hook      = defaultHook,
};

void* task2(void* a)
{
   float  pi=300.14;
   int p;
   while(1) {
      semphrTake ( &printfSemphr );                             // con este mutex me evito que si otra
      gpioToggle(LED2);
      pi/=2;
      if(pi>10) p=100;
      else p=200;
   }
   return NULL;
}
