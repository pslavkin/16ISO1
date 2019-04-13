#include <stdint.h>
#include "string.h"
#include "os.h"
#include "taskkernel.h"
#include "sapi.h"
#include "pendsv.h"
#include "systick.h"
#include "semphr.h"

bool semphrInit ( semphr_t* s, uint32_t count )
{
   s->count = 0;     // arranca tomado, a diferencia del mutex.
   s->max   = count; // cantidad maxima de gives..
};
//----------------------------------------------------------------------
bool semphrTake   ( semphr_t* m ){
   if(m->count>0) {                 // facil, si nadie lo tiene...
     m->count--;                    // lo tengo yo, con 1 unidad
   }
   else {
      tasks.context->state=BLOCKED; // ok, ocupado..me pongo en BLOCKED (no conundor con waiting)
      tasks.context->semphr=m;      // me guardo el puntero al semaforo que estoy esperando
      triggerPendSv();              // y aca libero el uC, de modo que a partir de ahora soy una tarea BLOQUEAD.. y ahora? quien podra rescatarme??
   }
};

bool semphrGive   ( semphr_t* m , uint32_t qty)
{
   int8_t i,j,k;
   bool yield=false;
   if(m->count<m->max) {            //si hay al menos un lugar, arranco
      if((m->count+qty)<m->max)     //si puedo dar todos los que me pidio, lo hago
         m->count+=qty;
      else
         m->count=m->max;                             // sino pongo lo maximo posible
      for (i=(MAX_PRIOR-1);m->count>0 && i>=0;i--) {  // recorro todas las prioridades pero SOLO hago yield si encuentro alguna tarea de mi misma o mayoyr que necesita el uC
         k=tasks.index[i];                            // auxiliar para recorrer desde la ultima posicion de cada lista de prioridades
         for(j=0;m->count>0 && j<MAX_TASK;j++) {      // barro todas las tareas del grupo de prioridad
            k=(k+1)%MAX_TASK;                         // incremento modulo MAX
            switch (tasks.list[i][k].state) {         // podria haber sido un if.. pero tengo otros planes
               case BLOCKED:                          // aja, encontre una..veamos si me esta esperando...
                  if(tasks.list[i][k].semphr==m) {    // si! me estaba esperando, le abro la puerta, si estaba bloqueada pero por otro semaforo, salteo
                     tasks.list[i][k].state  = READY; // aviso que esta taera pasa a ready
                     tasks.list[i][k].semphr = NULL;  // TODO: no se si hace falta..borro el puntero al semphr
                     m->count--;                      // decremento en uno el numero de semaforos
                     if(i>=tasks.context->prior)      // si la tarea que debloquie es de igual o mayor que la actual, tengo que hacer el yield! sino no. lo hace el kernel task luego, pero IGUAL la desbloqueo
                        yield = true;                 // aviso que voy a hacer yield, cuando termine de desbloquear todo
                  }
                  break;
               default:
                  break;
            }
         }
      }
   }
end:
   if(yield==true)
      taskYield();
   return yield;                                   //por ahora no uso la salida de esta func.
};

//----------------------------------------------------------------------
bool mutexInit ( semphr_t* m )
{
   m->count = 1; // arranca en 1, que seria desbloqueado en 1 unidad
   m->max   = 1; //
};
bool mutexLock ( semphr_t* s )
{
   return semphrTake(s);
}
bool mutexUnlock ( semphr_t* s )
{
   return semphrGive(s,1);
};

//-------------------------------------------
bool queueInit ( semphr_t* m ,circularBuffer_t* cb)
{
   m->count = 0;                       // arranca en 1, que seria desbloqueado en 1 unidad
   m->cb    = cb;
   m->max   = m->cb->amountOfElements; //
};
circularBufferStatus_t queueWrite ( semphr_t* s, void* data)
{
   circularBufferStatus_t ans = circularBufferWrite(s->cb,data);
   semphrGive(s,1);
}
circularBufferStatus_t queueRead ( semphr_t* s, void* data)
{
   semphrTake(s);
   return circularBufferRead(s->cb,data);
}
