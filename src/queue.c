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

semphr_t printfMutex;
void initPrintfMutex(void)
{
   mutexInit(&printfMutex);
}
bool queueInit ( queue_t* q ,circularBuffer_t* cb)
{
   q->s.count = 0;                         //  arranca en 1, que seria desbloqueado en 1 unidad
   q->s.max   = cb->amountOfElements-3;    //  -3.. todavia estoy pensando porque.. porque me da -2
   q->cb      = cb;                        //  guardo el puntero a la cola circular
};
bool queueWrite ( queue_t* q, void* data)
{
   return queueWriteTout ( q, data, 0);
}
bool queueWriteTout ( queue_t* q, void* data, uint32_t tout)
{
   mutexLock(&printfMutex);
      circularBufferWrite(q->cb,data);  // escribo el dato y por devuelvo como me fue..nuca deberia fallar porque acabo de hacer el give..
   mutexUnlock(&printfMutex);
   return semphrGiveTout(&q->s,1,tout); // intento hacer un give, pero si no hay lugar bloqueo.
}
bool queueRead ( queue_t* q, void* data)
{
   return queueReadTout ( q, data, 0);
}
bool queueReadTout ( queue_t* q, void* data, uint32_t tout)
{
   bool ans;
   ans=semphrTakeTout(&q->s,tout);                 //  bloqueo hasta que logre tomar el semaforo
   mutexLock(&printfMutex);
      circularBufferRead(q->cb,data); /// listo, tomado, leo la data y salgo
   mutexUnlock(&printfMutex);
   return ans;
}
