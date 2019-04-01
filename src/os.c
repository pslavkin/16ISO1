#include <stdint.h>
#include "string.h"
#include "os.h"
#include "taskkernel.h"
#include "sapi.h"
#include "taskidle.h"

taskContext    taskList[MAX_PRIOR][MAX_TASK];
taskContext    *runningContext;
uint8_t        taskCount[MAX_PRIOR]   = {0};

bool pushTask(taskParams* t, taskContext* c)
{
   *(--c->sp)=1<<24              ; // PSR                //comiezo a pushear todo ,bit de modo thumb en uno siempre
   *(--c->sp)=(uint32_t)t->func  ; // PC
   *(--c->sp)=(uint32_t)t->hook  ; // LR
   *(--c->sp)=0                  ; // r12

   *(--c->sp)=0                  ; // r3
   *(--c->sp)=0                  ; // r2
   *(--c->sp)=0                  ; // r1
   *(--c->sp)=(uint32_t)t->param ; // r0

   *(--c->sp)=0                  ; // r4
   *(--c->sp)=0                  ; // r5
   *(--c->sp)=0                  ; // r6
   *(--c->sp)=0                  ; // r7

   *(--c->sp)=0                  ; // r8
   *(--c->sp)=0                  ; // r9
   *(--c->sp)=0                  ; // r10
   *(--c->sp)=0                  ; // r11
}

bool taskFill(taskParams* t, taskContext* c, uint32_t prior)
{
      c->pool       = t->pool;
      c->sp         = &t->pool[t->pool_size];
      c->state      = READY;
      c->sleepTicks = 0;
      c->prior      = prior;
      strcpy   ( c->name ,t->name ); // la tarea guard su nombre
      pushTask ( t       ,c       );
      return true;
}

bool taskCreate(taskParams* t, uint32_t prior)
{
   if(taskCount[prior]<MAX_TASK)
      return taskFill(t,&taskList[prior][taskCount[prior]++],prior);
   else
      return false;
}
bool initTasks(void)
{
   uint8_t i,j;

   for(i=0;i<MAX_PRIOR;i++) {
      for(j=0;j<MAX_TASK;j++) {
         taskList[i][j].state=EMPTY;
      }
   }
   taskFill(&taskKernelParams,&kernelContext,0);
   runningContext=&kernelContext;         //wah? el primer salto se hace de main con el stack principal.. se lo regalo al kerneltask por ahora, 
   taskCreate ( &taskIdleParams ,0 );     //tarea idle
}
bool taskDelete(taskContext* c)
{
}
void yield(void)
{
   __ISB();
   __DSB();
   SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
bool taskDelay(uint32_t t)
{
   runningContext->sleepTicks = t;
   runningContext->state      = WAITING;
   yield();
}

