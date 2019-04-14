#include <stdint.h>
#include <stdio.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "task1.h"
#include "mutex.h"
#include "taskidle.h"
#include "taskkernel.h"
#include "taskprint.h"

void printTaskStat(taskContext_t* t)
{
   uint8_t data[MAX_MSG_LENGTH];
   uint8_t* status[]= {
      "ready",
      "waiting",
      "running",
      "empty",
      "blocked_take",
      "blocked_give"
   };

   sprintf(data,"%s-%s-%d-%d-%d\r\n",
         t->name,
         status[t->state],
         t->prior,
         t->waterMark,
         t->runCount
         );
   queueWrite(&printQueue,data);
}
void printTasksStat(tasks_t* t)                             //imprimie la estadisticas de todas las tareas en la lista que no sen empty
{
   int8_t i,j;                                                 // uso indices signados porque voy a comparar con >=0. se podria hacer tambien de otra manera
   queueWrite(&printQueue,"name-state-prior-stack-run\r\n");
   for (i=(MAX_PRIOR-1);i>=0;i--) {                            // arranca siempre desde la maxima prioridad
      for(j=0;j<MAX_TASK;j++) {                                // barro todas las tareas del grupo de prioridad
         if(t->list[i][j].state!=EMPTY)
            printTaskStat(&t->list[i][j]);
      }
   }
   printTaskStat ( &idleContext   );   //si, idel tambien es una tarea.
   printTaskStat ( &kernelContext );   //y tambien el kernel
}
int32_t stampWaterMark(taskContext_t* t)  //pone la peor marca del stack, la que estuvo mas cercad de desbordar
{
   if (t->waterMark>(t->sp-t->pool))
      t->waterMark=t->sp-t->pool;
   return t->waterMark;
}
void stampRunCount(taskContext_t* t)   //incrementa el numero de corridas
{
   t->runCount++;
}
