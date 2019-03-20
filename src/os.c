#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"

taskContext*   taskList[10];
uint32_t       taskCount   = 0;
uint32_t       runningTask = 0;
taskContext    *runningContext;
taskContext    kernelContext;

bool stackPush(uint32_t data, taskContext* c)
{
   c->pool_index--;
   *c->pool_index=data;
}
bool pushTask(const char* name, void* (*func)(void*),void* param, taskContext* c, void* (*hook)(void*))
{
   strcpy(c->name,name);                         // la tarea guard su nombre
   c->pool_index=&(c->pool[MIN_STACK]);          // el pool es estativo y fijo, se guarda el puntero a la ultima posicion porque funcionara como stak de arriba para abao
   stackPush ( 1<<24                       ,c ); // PSR     //comiezo a pushear todo, bit de modo thumb en uno siempre
   stackPush ( (uint32_t)func              ,c ); // PC
   stackPush ( (uint32_t)hook              ,c ); // LR
   stackPush ( 0                           ,c ); // r12

   stackPush ( 0                           ,c ); // r3
   stackPush ( 0                           ,c ); // r2
   stackPush ( 0                           ,c ); // r1
   stackPush ( (uint32_t)param             ,c ); // r0

   stackPush ( 0                           ,c ); // r4
   stackPush ( 0                           ,c ); // r5
   stackPush ( 0                           ,c ); // r6
   stackPush ( 0                           ,c ); // r7

   stackPush ( 0                           ,c ); // r8
   stackPush ( 0                           ,c ); // r9
   stackPush ( 0                           ,c ); // r10
   stackPush ( 0                           ,c ); // r11
}

bool taskCreate(const char* name, void* (*func)(void*),void* param, taskContext* context, void* (*hook)(void*))
{
   pushTask(name,func,param,context,hook);
   taskList[taskCount++]=context;                      // habemos una tarea extra
}
bool kernelCreate(const char* name, void* (*func)(void*),void* param, taskContext* context, void* (*hook)(void*))
{
   pushTask(name,func,param,context,hook);
   runningTask    = 0;
   runningContext = &kernelContext;
}
bool taskDelete(taskContext* c)
{
  taskCount--;    //tarea menos
}

void* kernelTask(void* p) //round robin por ahora (pero con hook)
{
   if(++runningTask>=taskCount)
      runningTask=0;
   runningContext=taskList[runningTask];
}
void* hookKernel(void* p)
{
   return NULL;
}
