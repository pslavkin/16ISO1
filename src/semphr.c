#include <stdint.h>
#include "os.h"
#include "taskkernel.h"
#include "sapi.h"
#include "pendsv.h"
#include "semphr.h"

bool semphrInit ( semphr_t* s, uint32_t count )
{
   s->count = 0;     // arranca tomado, a diferencia del mutex.
   s->max   = count; // cantidad maxima de gives..
};
//----------------------------------------------------------------------
bool semphrTake   ( semphr_t* s )
{
   bool yield=false;
   if(s->count>0) {                         // si alguien hizo el give antes, o nadie tiene el mutex
      yield=freeBlockedGived(s);            // cuando se hace el take reviso si no quedo algun intento de give bloqueado por falta de espacio y lo libero. solo uno, y si es de prioridad mas alta que yo, entonces le cedo el uC
      s->count--;                           // me quedo con 1 unidad
   }
   else {
      tasks.context->event = s;            // me guardo el puntero al semaforo que estoy esperando
      tasks.context->state  = BLOCKED_TAKE; // ok, ocupado..me pongo en BLOCKED_TAKE (no conundor con waiting)
      triggerPendSv();                      // y aca libero el uC, de modo que a partir de ahora soy una tarea BLOQUEAD.. y ahora? quien podra rescatarme??
   }
   if(yield==true)                          // si se puse en ready al menos una tarea de igual o mayor prioridad cedo el uC
      taskYield();
   return yield;
};

bool semphrGive   ( semphr_t* s , uint32_t qty)
{
   int8_t i,j,k;
   bool yield=false;
   if(s->count<s->max) {                   // si hay al menos un lugar, arranco
      s->count+=qty;                       // sumo la cantidad qty que se pide
      if(s->count>s->max)                  // ojo de no pasarme del maximo.. si me pase. trunco
         s->count=s->max;                  // trunco a lo maximo
      yield=freeBlockedTaked(s);           // y antes de termina, libero a TODOS los que esten esperando este sempht, siempre y cuando no llegue a cero el count. Y de paso si hubo alguno de mayor prioridad, le cedo el uC
   }
   else {
      stdioPrintf(UART_USB,"BLOCKED_GIVE\r\n"); //debug

      tasks.context->event = s;            // me guardo el puntero al semaforo que estoy esperando
      tasks.context->state = BLOCKED_GIVE; // ok, ocupado..me pongo en BLOCKED_GIVE (no conundor con waiting)
      triggerPendSv();                     // y aca libero el uC, de modo que a partir de ahora soy una tarea BLOQUEAD.. y ahora? quien podra rescatarme??
   }
   if(yield==true)                         // si hubo al menos una tarea de mayour prioridad liberada, le cedo el uC
      taskYield();
   return yield;                           // por ahora no uso la salida de esta func.
};
                                            // ----------------------------------------------------------------------
