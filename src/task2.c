#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "semphr.h"
#include "event.h"
#include "mutex.h"
#include "systick.h"
#include "task1.h"
#include "task2.h"
#include "taskprint.h"
#include "stat.h"
#include "convert.h"

uint32_t task2Pool[REASONABLE_STACK];

semphr_t task2Semphr;
event_t  task2Event;

void task2Init(void)
{
   semphrInit ( &task2Semphr,5 );
   eventInit  ( &task2Event,5  );
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

task1QueueStruct_t* t1_copy;
uint8_t t1_static[10];
void* task2(void* a)
{
   while(1) {
      eventTake(&task1Event,(void*)&t1_copy);
      uint32_t i;
      for(i=0;i<TASK1_BUFFER_SIZE;i++) {
         t1_copy->buf[i]=(i%10)+(t1_copy->actual==0?'0':'A');
      }
      t1_copy->buf[i-3]='\r';
      t1_copy->buf[i-2]='\n';
      t1_copy->buf[i-1]='\0';
      gpioToggle ( LED2 );
      eventGive(&task2Event,t1_copy->buf,1);
//      taskDelay(msec2Ticks(1000));
//      queueRead(&task1Queue,&t1_static);
   }
   return NULL;
}
