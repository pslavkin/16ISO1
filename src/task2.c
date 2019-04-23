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
#include "convert.h"

uint32_t task2Pool[REASONABLE_STACK];

semphr_t task2Semphr;

void task2Init(void)
{
   semphrInit(&task2Semphr,5);
}

taskParams_t task2Params = {
   .name      = "task2",
   .pool      = task2Pool,
   .poolSize  = sizeof(task2Pool)/sizeof(task2Pool[0]),
   .param     = NULL,
   .func      = task2,
   .hook      = defaultHook,
   .init      = task2Init,
};

task1QueueStruct_t t1_copy;
void* task2(void* a)
{
   while(1) {
      queueRead( &task1Queue, &t1_copy);//,msec2Ticks(1000) );
      uint32_t i;
      for(i=0;i<TASK1_BUFFER_SIZE;i++) {
         t1_copy.buf[i]=(i%10)+(t1_copy.actual==0?'0':'A');
      }
      t1_copy.buf[i-3]='\r';
      t1_copy.buf[i-2]='\n';
      t1_copy.buf[i-1]='\0';
      gpioToggle ( LED2 );
      semphrGive(&task2Semphr,1);
      queueWrite ( &printQueue,t1_copy.buf);
   }
   return NULL;
}
