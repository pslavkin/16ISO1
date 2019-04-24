#ifndef TASK1
#define TASK1

#include "semphr.h"
#include "queue.h"

#define TASK1_BUFFER_SIZE 10
extern queue_t       task1Queue;
extern event_t       task1Event;
extern taskParams_t  task1Params;

typedef struct task1QueueStruct_struct
{
   uint8_t * buf;
   uint8_t actual;
} task1QueueStruct_t;

void* task1            ( void* a );
#endif
