#include <stdint.h>
#include <stdlib.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "mutex.h"
#include "semphr.h"
#include "event.h"
#include "queue.h"
#include "systick.h"
#include "task1.h"
#include "tasktemplate.h"
#include "taskprint.h"
#include "stat.h"
#include "taskkey.h"
//------------------------------------------
#define TASK1_MSG_QTY      10
#define TASK1_MSG_LENGTH   (sizeof(keys_t))

int8_t  task1CbPool [cbPoolSpace(TASK1_MSG_QTY,TASK1_MSG_LENGTH)];
circularBuffer_t t1Cb;
queue_t task1Queue;
event_t ledEvent;

uint32_t task1Pool[REASONABLE_STACK];

void task1Init(void)
{
   queueInit ( &task1Queue,&t1Cb,task1CbPool,TASK1_MSG_QTY,TASK1_MSG_LENGTH );
   eventInit(&ledEvent,1);
}
//------------------------------------------
uint32_t task1Pool[REASONABLE_STACK];

taskParams_t task1Params = {
   .name      = "task1",
   .pool      = task1Pool,
   .poolSize  = sizeof(task1Pool)/sizeof(task1Pool[0]),
   .param     = NULL,
   .func      = task1,
   .hook      = defaultHook,
   .init      = task1Init,
};

const uint8_t* colors_text[4] = {
   "verde",
   "rojo",
   "amarillo",
   "azul"
};

void calcKeyDeltas(timing_t* t,keys_t* k)
{
   t->deltaT1 = k->fallTime[1]-k->fallTime[0];
   t->deltaT2 = k->riseTime[1]-k->riseTime[0];
   t->sumT1T2 = abs(t->deltaT1)+abs(t->deltaT2);
   t->color   = (t->deltaT1<0?2:0) + (t->deltaT2<0);
}
void printKeyReport(timing_t* t,keys_t* keys)
{
   printUART("led %s encendido:\r\n"
         "\t tiempo encendido: %d ms\r\n"
         "\t tiempo entre flancos descendentes: %d ms\r\n"
         "\t tiempo entre flancos ascendentes : %d ms\r\n",
         colors_text[t->color],
         ticks2msec(t->sumT1T2),
         ticks2msec(t->deltaT1),
         ticks2msec(t->deltaT2)
         );
}

void* task1(void* a)
{
   keys_t   keys;
   timing_t timing;
   while(1) {
         queueRead      ( &task1Queue ,&keys );
         calcKeyDeltas  ( &timing     ,&keys );
         eventGive ( &ledEvent,(void*)&timing,1);
         printKeyReport ( &timing     ,&keys );
   }
   return NULL;
}
