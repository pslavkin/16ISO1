#ifndef TASK1
#define TASK1

#include "semphr.h"
#include "queue.h"
#include "taskkey.h"

enum colors_enum {
   VERDE=0,
   ROJO,
   AMARILLO,
   AZUL
};
typedef struct timing_struct
{
   int32_t   deltaT1;
   int32_t   deltaT2;
   uint32_t  sumT1T2;
   uint8_t   color;
}timing_t;

extern event_t ledEvent;
extern queue_t       task1Queue;
extern taskParams_t  task1Params;
void calcKeyDeltas  ( timing_t* t,keys_t* k    );
void printKeyReport ( timing_t* t,keys_t* keys );

void* task1            ( void* a );
#endif
