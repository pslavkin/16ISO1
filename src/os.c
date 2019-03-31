#include <stdint.h>
#include "string.h"
#include "os.h"
#include "taskkernel.h"
#include "sapi.h"

taskContext    taskList[MAX_TASK];
uint32_t       taskCount   = 0;
uint32_t       tick        = 0;

bool stackPush(uint32_t data, taskContext* c)
{
   c->sp--;
   *c->sp=data;
}
bool pushTask(taskParams* t, taskContext* c)
{
   stackPush ( 1<<24              ,c ); // PSR     //comiezo a pushear todo ,bit de modo thumb en uno siempre
   stackPush ( (uint32_t)t->func  ,c ); // PC
   stackPush ( (uint32_t)t->hook  ,c ); // LR
   stackPush ( 0                  ,c ); // r12

   stackPush ( 0                  ,c ); // r3
   stackPush ( 0                  ,c ); // r2
   stackPush ( 0                  ,c ); // r1
   stackPush ( (uint32_t)t->param ,c ); // r0

   stackPush ( 0                  ,c ); // r4
   stackPush ( 0                  ,c ); // r5
   stackPush ( 0                  ,c ); // r6
   stackPush ( 0                  ,c ); // r7

   stackPush ( 0                  ,c ); // r8
   stackPush ( 0                  ,c ); // r9
   stackPush ( 0                  ,c ); // r10
   stackPush ( 0                  ,c ); // r11
}

bool initTasks(void)
{
   uint8_t i;
   for(i=0;i<MAX_TASK;i++) {
      taskList[i].state=EMPTY;
   }
   runningContext=&taskList[8];
}
bool taskCreate(taskParams* t, uint32_t prior)
{
   taskContext* c;
   if(taskCount>=MAX_TASK)
      return false;
   else {
      c             = &taskList[taskCount];
      c->pool       = t->pool;// el pool es estativo y fijo, se guarda el puntero a la ultima posicion porque funcionara como stak de arriba para abao
      c->sp         = &t->pool[t->pool_size];
      c->state      = ACTIVE;
      c->sleepTicks = 0;
      c->prior      = prior;
      strcpy(c->name,t->name); // la tarea guard su nombre
      pushTask(t,c);
      taskCount++;
   }
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
   yield();
   tick++;
}

