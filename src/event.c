#include <stdint.h>
#include "os.h"
#include "taskkernel.h"
#include "systick.h"
#include "sapi.h"
#include "pendsv.h"
#include "event.h"

bool eventInit ( event_t* s, uint32_t count )
{
   s->count = 0;     // arranca tomado, a diferencia del mutex.
   s->max   = count; // cantidad maxima de gives..
   s->data  = NULL;  // arranco con un dato NULL
   return true;
};
//----------------------------------------------------------------------
bool eventTake   ( event_t* s, void** data)
{
   return eventTakeTout   ( s, data, 0 );
}
bool eventTakeTout   ( event_t* s, void** data, uint32_t tout )
{
   if(s->count==0) {
      tasks.context->event     = s;                 // me guardo el puntero al semaforo que estoy esperando
      tasks.context->eventAns  = false;             // me guardo el puntero al semaforo que estoy esperando
      tasks.context->eventTout = tout>0;            // aca decido si es forever o tiene un tout
      tasks.context->sleep     = tout>0?(tout-1):0; // tiempo asignado al tout
      tasks.context->state     = BLOCKED_TAKE;      // ok, ocupado..me pongo en BLOCKED_TAKE (no conundor con waiting)
      triggerPendSv();                              // y aca libero el uC, de modo que a partir de ahora soy una tarea BLOQUEAD.. y ahora? quien podra rescatarme??
      if(data!=NULL)                                // con esto lo que hago es permitir pasar un dato, un puntero a void, durante el envio del evento, es muy util para psar punteros a estructuras sin necesidad de armar una cola
         *data=tasks.context->eventData;            // transfiero el dato al usuario
      return tasks.context->eventAns;               // aviso que respondi para que el usuario sepa si es valido o no
   }
   else {
      s->count--;                                   // me quedo con 1 unidad
      if(data!=NULL)                                // si esta mandando datos
         *data=s->data;                             // cargo el dato para que quede disponible en el destinatario
   }
   if(freeBlockedGived(s)==true)                    // si hubo al menos una tarea de mayour prioridad liberada, le cedo el uC
      taskYield();
   return true;
};

bool eventGive   ( event_t* s , void* data, uint32_t qty)
{
   eventGiveTout ( s , data, qty ,0 );
}
bool eventGiveTout   ( event_t* s, void* data, uint32_t qty, uint32_t tout)
{
   tasks.context->sleep     = tout>0?(tout-1):0; // fuera del while para que si se activa con un parcial de datos que no me alcanza para liberar el give, NO inicializo de nuevo el tput.. sigue descontando (si es que es un bloque con tout..)
   tasks.context->eventTout = tout>0;            // aca decido si es forever o tiene un tout
   tasks.context->event     = s;                 // me guardo el puntero al semaforo que estoy esperando
   tasks.context->eventAns  = true;              // lo inicializo en true, porque lo uso apra saber si se puso en ready por tout o porque hay espacio nuevo
   while((s->count+qty)>s->max) {                // es un while, porque tengo que esperar a que se libere espacio, mientras no lo tenga, me quedo lupeando o salgo por tout
      tasks.context->state     = BLOCKED_GIVE;   // ok, ocupado..me pongo en BLOCKED_GIVE (no conundor con waiting)
      triggerPendSv();                           // y aca libero el uC, de modo que a partir de ahora soy una tarea BLOQUEAD.. y ahora? quien podra rescatarme??
      if(tasks.context->eventAns==false)         // si llego aca es porque no hubo chance de meter todos los gives pedidos..
         return false;                           //
   }
   s->data   = data;
   s->count += qty;                              // sumo la cantidad qty que se pide
   if(freeBlockedTaked(s)==true)                 // si hubo al menos una tarea de mayour prioridad liberada, le cedo el uC
      taskYield();
   return true;                                  // por ahora no uso la salida de esta func.
};
//esta api es diferente desde la isr porque por ejemplo nunca puede bloquear ni tener tout ni
//nada, es otro contexto y lo manejo diferente...
bool eventGiveTout4Isr  ( event_t* s, void* data, uint32_t qty)
{
   if((s->count+qty)<=s->max) {   // si no entra decido no mandar ninguno.. raro, pero bueno, prefiero nada a incompleto, pero es discutible
      s->data   = data;           // cargo el dato del evento
      s->count += qty;            // sumo la cantidad qty que se pide
      return freeBlockedTaked(s); // libero las tareas que estaban esperando este smaforo pero NO llamo al schedule, lo decide la irq antes de salira
   }
   return false;
};
bool eventGiveBlock   ( event_t* s,uint32_t tout)
{
   tasks.context->sleep     = tout>0?(tout-1):0; // fuera del while para que si se activa con un parcial de datos que no me alcanza para liberar el give, NO inicializo de nuevo el tput.. sigue descontando (si es que es un bloque con tout..)
   tasks.context->eventTout = tout>0;            // aca decido si es forever o tiene un tout
   tasks.context->event     = s;                 // me guardo el puntero al semaforo que estoy esperando
   tasks.context->eventAns  = true;              // lo inicializo en true, porque lo uso apra saber si se puso en ready por tout o porque hay espacio nuevo
   tasks.context->state     = BLOCKED_GIVE;      // ok, ocupado..me pongo en BLOCKED_GIVE (no conundor con waiting)
   triggerPendSv();                              // y aca libero el uC, de modo que a partir de ahora soy una tarea BLOQUEAD.. y ahora? quien podra rescatarme??
   if(tasks.context->eventAns==false)            // si llego aca es porque no hubo chance de meter todos los gives pedidos..
      return false;
   return true;                                  // por ahora no uso la salida de esta func.
};
// ----------------------------------------------------------------------
