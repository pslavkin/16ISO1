#include <stdint.h>
#include "string.h"
#include "os.h"
#include "taskkernel.h"
#include "sapi.h"
#include "pendsv.h"
#include "systick.h"
#include "semphr.h"
#include "mutex.h"
#include "queue.h"

bool queueInit ( queue_t* q ,
                  circularBuffer_t* cb,
                  uint8_t* bufferMemory,       // buffer array of memory
                  uint32_t amountOfElements,   // amount of elements in buffer
                  uint32_t elementSize         // each element size in bytes
               )
{
   circularBuffer_Init ( cb ,bufferMemory ,amountOfElements ,elementSize );
   q->s.count = 0;                  // arranca en 1, que seria desbloqueado en 1 unidad
   q->s.max   = amountOfElements; // reservo uno para mi porque PRIMERO escribo y luego doy ek semphr con lo cual tengo que tener siempre uno libre
   q->cb      = cb;                 // guardo el puntero a la cola circular
   mutexInit(&q->mutex);            // inicializo el mutex que usa la cola para protejer el acceso al buffer circular
};
bool queueWrite ( queue_t* q, void* data)
{
   return queueWriteTout ( q, data, 0);
}
bool queueWriteTout ( queue_t* q, void* data, uint32_t tout)
{
   //aca tuve que hacer un workaroud, por el siguiente caso.
   //Si al buffer le queda un elemento, entro y cargo los datos, hago el give y me trabo porque
   //justo cuando al buffer le queda uno, el give esta lleno. ELuego otra tarea de menor
   //prioridad hace el take y me libera, yo sigo y vuelvo a hacer un queuewrite, pero AUN no
   //han sacado nada del buffer porque tengo mayor prioridad.. para lo cual lo que hago es
   //escribir y preguntar si se pudo escribir. si se pudo, genial sino, yield y espero.
   //con este esquema en realidad el numero de semaforos y numero de elemenots del buffer no
   //tiene porque ser coicindentes, porque se traban mutuamente. SI element es 2 y semaforos
   //1000, solo voy a poder escribir de a 2 elementos. en cambio si tengo 2 semaforos y 1000
   //elementos, tambien puedo solo madar 2..el optimo es que sea un semaforo menos que elemento
   //por el hecho de que primero escribo y luego hago give. Pero para permitir que se puedan
   //implementar colas de 1 solo elemento, voy a dejarlos igual semaforo y elementos de la cola
   circularBufferStatus_t cbState;         
   do {
      mutexLock(&q->mutex);                // protejo el buffer circular
         cbState=circularBufferWrite(q->cb,data);  // PRIMERO grabo. estoy seguro de que puedo porque la cola tiene un lugar vacio extra con respecto al semaforo.Lo hago asi porque si el Give funciona, ya despierta a las tareas que usaran el dato del buffer circular, asi que tiene que estar ya cargado..0 
      mutexUnlock(&q->mutex);
      if(cbState!=CIRCULAR_BUFFER_FULL)
         return semphrGiveTout(&q->s,1,tout); // POR ULTIMO, intento hacer un give, pero si no hay lugar bloqueo.
      taskYield();
   } while (cbState==CIRCULAR_BUFFER_FULL);
}
bool queueRead ( queue_t* q, void* data)
{
   return queueReadTout ( q, data, 0);
}
bool queueReadTout ( queue_t* q, void* data, uint32_t tout)
{
   bool ans;
   ans=semphrTakeTout(&q->s,tout);    //  PRIMERO bloqueo hasta que logre tomar el semaforo
   mutexLock(&q->mutex);              //  protejo el buffer circular
      circularBufferRead(q->cb,data); /// listo, tomado, leo la data y salgo
   mutexUnlock(&q->mutex);
   return ans;
}
