#include <stdint.h>
#include "os.h"
#include "taskkernel.h"
#include "systick.h"
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
   if(s->count==0) {
      tasks.context->event = s;            // me guardo el puntero al semaforo que estoy esperando
      tasks.context->state = BLOCKED_TAKE; // ok, ocupado..me pongo en BLOCKED_TAKE (no conundor con waiting)
      triggerPendSv();                     // y aca libero el uC, de modo que a partir de ahora soy una tarea BLOQUEAD.. y ahora? quien podra rescatarme??
   }
   else {
      s->count--;                             // me quedo con 1 unidad
   }
   if(freeBlockedGived(s)==true)           // si hubo al menos una tarea de mayour prioridad liberada, le cedo el uC
      taskYield();
   return true;
};

bool semphrGive   ( semphr_t* s , uint32_t qty)
{
   while(s->count>=s->max) {
      tasks.context->event = s;            // me guardo el puntero al semaforo que estoy esperando
      tasks.context->state = BLOCKED_GIVE; // ok, ocupado..me pongo en BLOCKED_GIVE (no conundor con waiting)
      triggerPendSv();                     // y aca libero el uC, de modo que a partir de ahora soy una tarea BLOQUEAD.. y ahora? quien podra rescatarme??
   }
   s->count+=qty;                          // sumo la cantidad qty que se pide
   if(s->count>s->max)                     // ojo de no pasarme del maximo.. si me pase. trunco
      s->count=s->max;                     // trunco a lo maximo
   if(freeBlockedTaked(s)==true)           // si hubo al menos una tarea de mayour prioridad liberada, le cedo el uC
      taskYield();
   return true;                            // por ahora no uso la salida de esta func.
};
// ----------------------------------------------------------------------
