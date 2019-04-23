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
#include "convert.h"

void printTaskStat(taskContext_t* t)
{
   uint8_t data[MAX_MSG_LENGTH];
   //estados posibles, sincronizar con el enum taskState de os.c
   uint8_t* status[]= {
      "ready",
      "waiting",
      "running",
      "empty",
      "deleted",
      "blocked_take",
      "blocked_give"
   };

   sprintf(data,"%14s | %12s | %5d | %5d | %5d/%5d | %8d | %8s%",
         t->name,          // nombre de fantasia
         status[t->state], // estado
         t->sleep, // sleep pendiente
         t->prior,         // prioridad de la tarea
         t->waterMark*4,   // minima distancia entre el inicio del pool y por donde esta el sp
         t->poolSize*4,    // minima distancia entre el inicio del pool y por donde esta el sp
         t->runCount,      // numero de veces que fue atendida por el scheduler
         //aca viene la injusticia... solo es cualitativa la medicino (aunque anda muy bien
         //tengo que decir) pero solo toma la relacion de veces que fue atendida versus la
         //cantidad de veces que fue llamado el cambio de contexto. Pero por ejemplo si una
         //tarea es llamada muchas veces pero no hace nada, solo cede el CPU, y otro es llamada
         //cada mucho tiempo y usa mucho CPU, estaria midiendo injustamente.. por otra parte si
         //la tarea es borrada en tiempo de ejecucion, se pierde la relacion. Por eso muestro
         //tambein en la tabla las tareas que fueron borradas.. hasta que alguna creacino de
         //tareas on-runtime no tenga mas lugar y ahi la pista y ahi si se pierde la relacion
         //matematica entre numero de atenciones del kernel y sumatoria de atenciones de todas
         //las tareas, porque hay una tarea borrada que usao tiempo y no figura mas.. 
         //para resolverlo se puede resetear todos los contadores o descontar, o algo.. decido
         //no hacer nada , es un caso muyyy border...
         ftostr(((float)t->runCount*100)/kernelContext.runCount,data+MAX_MSG_LENGTH-15)
         );
   queueWrite(&printQueue,data);
}
void printTasksStat(tasks_t* t)                             //imprimie la estadisticas de todas las tareas en la lista que no sen empty
{
   int8_t i,j;                                                 // uso indices signados porque voy a comparar con >=0. se podria hacer tambien de otra manera
   uint8_t data[MAX_MSG_LENGTH];
   sprintf(data,"%14s | %12s | %5s | %5s | %11s | %8s | %%%4s\r\n"
                "---------------------------------------------------------------------------------\r\n",
         "name    ",
         "state   ",
         "sleep",
         "prior",
         "stack/mem ",
         "run   ",
         "use"
         );
   queueWrite(&printQueue,data);
   for (i=(MAX_PRIOR-1);i>=0;i--) {        // arranca siempre desde la maxima prioridad
      for(j=0;j<MAX_TASK;j++) {            // barro todas las tareas del grupo de prioridad
         if(t->list[i][j].state!=EMPTY)    // muestra en la tabla las que no estan empty. Ojo que si muestra las DELETED
            printTaskStat(&t->list[i][j]); // imprime la linea con los datos de dicha tarea..
      }
   }
   printTaskStat ( &idleContext   );       // si, idel tambien es una tarea.
   printTaskStat ( &kernelContext );       // y tambien el kernel
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
