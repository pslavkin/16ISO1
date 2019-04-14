#include <stdint.h>
#include "string.h"
#include "os.h"
#include "taskkernel.h"
#include "sapi.h"
#include "pendsv.h"
#include "systick.h"
#include "semphr.h"
#include "queue.h"

bool queueInit ( queue_t* q ,circularBuffer_t* cb)
{
   q->s.count = 0;                         //  arranca en 1, que seria desbloqueado en 1 unidad
   q->s.max   = cb->amountOfElements;      //  no puedo dar mas gives que la canridad de elementos de la cola
   q->cb      = cb;                        //  guardo el puntero a la cola circular
};
circularBufferStatus_t queueWrite ( queue_t* q, void* data)
{
   semphrGive(&q->s,1);                    //  intento hacer un give, pero si no hay lugar bloqueo.
   return circularBufferWrite(q->cb,data); //  escribo el dato y por devuelvo como me fue..nuca deberia fallar porque acabo de hacer el give..
}
circularBufferStatus_t queueRead ( queue_t* q, void* data)
{
   semphrTake(&q->s);                      //  bloqueo hasta que logre tomar el semaforo
   return circularBufferRead(q->cb,data);  /// listo, tomado, leo la data y salgo
}
