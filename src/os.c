#include <stdint.h>
#include "string.h"
#include "os.h"
#include "taskkernel.h"
#include "sapi.h"
#include "taskidle.h"

taskContext    taskList[MAX_TASK];
uint32_t       taskCount   = 0;
uint32_t       tick        = 0;

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
      c->state      = ACTIVE;
      c->sleepTicks = 0;
      c->prior      = prior;
      strcpy(c->name,t->name); // la tarea guard su nombre
      pushTask(t,c);
}

bool taskCreate(taskParams* t, uint32_t prior)
{
   if(taskCount>=MAX_TASK)
      return false;
   else
      taskFill(t,&taskList[taskCount++],prior);
}
bool initTasks(void)
{
   uint8_t i;
   for(i=0;i<MAX_TASK;i++) {
      taskList[i].state=EMPTY;
   }
   taskFill(&taskKernelParams,&kernelContext,0);
   runningContext=&kernelContext;           //waf? el primer salto se hace de main con el stack principal.. se lo regalo al kerneltask por ahora, 
   taskCreate ( &taskIdleParams ,1 );     //tarea idle
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
   runningContext->state      = SLEEPING;
   yield();
}

void Init_SysTick(void)
{
   SysTick_Config( (SystemCoreClock * (tick_t)50) / 100 );
   yield();
}
void SysTick_Handler(void)
{
   tick++;
   uint8_t i;

   for(i=0;i<MAX_TASK;i++) {
      switch (taskList[i].state) {
         case SLEEPING:
            if(--taskList[i].sleepTicks == 0)
               taskList[i].state = ACTIVE;
            break;
         default:
            break;
      }
   }
   yield();
}

