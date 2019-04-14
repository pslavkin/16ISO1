#ifndef QUEUE
#define QUEUE

#include <stdint.h>
#include <stdbool.h>
#include "sapi.h"

typedef struct    queue_struct { // ok, si por ahora esta estructura no es muy util.. pero tengo otros planes...
   event_t            s ;
   circularBuffer_t  *cb;
} queue_t;

bool                    queueInit  ( queue_t* q ,circularBuffer_t* cb );
circularBufferStatus_t  queueWrite ( queue_t* q, void* data           );
circularBufferStatus_t  queueRead  ( queue_t* q, void* data           );

#endif
