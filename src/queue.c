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
   q->s.max   = amountOfElements-1; // reservo uno para mi porque PRIMERO escribo y luego doy ek semphr con lo cual tengo que tener siempre uno libre
   q->cb      = cb;                 // guardo el puntero a la cola circular
   mutexInit(&q->mutex);            // inicializo el mutex que usa la cola para protejer el acceso al buffer circular
};
bool queueWrite ( queue_t* q, void* data)
{
   return queueWriteTout ( q, data, 0);
}
bool queueWriteTout ( queue_t* q, void* data, uint32_t tout)
{
   mutexLock(&q->mutex);                // protejo el buffer circular
      circularBufferWrite(q->cb,data);  // PRIMERO grabo. estoy seguro de que puedo porque la cola tiene un lugar vacio extra con respecto al semaforo.Lo hago asi porque si el Give funciona, ya despierta a las tareas que usaran el dato del buffer circular, asi que tiene que estar ya cargado..0 
   mutexUnlock(&q->mutex);
   return semphrGiveTout(&q->s,1,tout); // POR ULTIMO, intento hacer un give, pero si no hay lugar bloqueo.
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
