#ifndef QUEUE
#define QUEUE

#include <stdint.h>
#include <stdbool.h>
#include "sapi.h"

typedef struct    queue_struct { // ok, si por ahora esta estructura no es muy util.. pero tengo otros planes...
   event_t            s ;
   circularBuffer_t  *cb;
} queue_t;

bool queueInit      ( queue_t* q ,circularBuffer_t* cb      );
bool queueWriteTout ( queue_t* q, void* data, uint32_t tout );
bool queueReadTout  ( queue_t* q, void* data, uint32_t tout );
bool queueWrite     ( queue_t* q, void* data);
bool queueRead      ( queue_t* q, void* data);

#endif
