#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "mutex.h"
#include "semphr.h"
#include "queue.h"
#include "systick.h"
#include "task1.h"
#include "tasktemplate.h"
#include "taskprint.h"
#include "stat.h"

//------------------------------------------

#define TASK1_MSG_QTY 10
#define TASK1_MSG_LENGTH (sizeof(task1QueueStruct_t))
#define TASK1_BUFFER_SIZE 10

int8_t  cbPool [(TASK1_MSG_QTY)*TASK1_MSG_LENGTH];
circularBuffer_t t1Cb;
queue_t task1Queue;

uint8_t buf[2][TASK1_BUFFER_SIZE];
task1QueueStruct_t t1={
      .buf=buf[0],
      .actual=0
};

uint32_t task1Pool[REASONABLE_STACK];

void task1Init(void)
{
   queueInit ( &task1Queue,&t1Cb,cbPool,TASK1_MSG_QTY,TASK1_MSG_LENGTH );
}
//------------------------------------------
uint32_t task1Pool[REASONABLE_STACK];

taskParams_t task1Params = {
   .name      = "task1",
   .pool      = task1Pool,
   .poolSize  = sizeof(task1Pool)/sizeof(task1Pool[0]),
   .param     = NULL,
   .func      = task1,
   .hook      = defaultHook,
   .init      = task1Init,
};

void* task1(void* a)
{
   while(1) {
         taskDelay(msec2Ticks(20 ));
         t1.buf    = buf[0];
         t1.actual = 0;
         queueWrite(&task1Queue,&t1);

         taskDelay(msec2Ticks(20));
         t1.buf    = buf[1];
         t1.actual = 1;
         queueWrite(&task1Queue,&t1);
   }
   return NULL;
}
