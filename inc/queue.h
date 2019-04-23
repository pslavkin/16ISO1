#ifndef QUEUE
#define QUEUE

#include <stdint.h>
#include <stdbool.h>
#include "sapi.h"
#include "mutex.h"

typedef struct    queue_struct { // ok, si por ahora esta estructura no es muy util.. pero tengo otros planes...
   event_t            s ;        //es el evento para leer y escribir la cola.
   circularBuffer_t  *cb;        //es el puntero a un tipo colacircular de la sapi
   mutex_t           mutex;      //uso un mutex para protejer el acceso al buffer circualr de la sapi porque sino podria darse concurrencia
} queue_t;

bool queueInit ( queue_t* q ,
                  circularBuffer_t* cb,
                  uint8_t* bufferMemory,       // buffer array of memory
                  uint32_t amountOfElements,   // amount of elements in buffer
                  uint32_t elementSize         // each element size in bytes
               );
bool queueWriteTout ( queue_t* q, void* data, uint32_t tout );
bool queueReadTout  ( queue_t* q, void* data, uint32_t tout );
bool queueWrite     ( queue_t* q, void* data);
bool queueRead      ( queue_t* q, void* data);

#endif
