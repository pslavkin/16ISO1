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
#include "task2.h"
#include "tasktemplate.h"
#include "taskprint.h"
#include "stat.h"
#include "taskkey.h"
//------------------------------------------
////hay que ccalcularlo en funcion de cuantos mensaje podrian generarse durante la ejecucion de
//la demora de los leds., por ejemplo si hay una operacion de encendido de leds de 1hora,
//podrian seguir generandose eventos de presionado de teclas y se llenaria esta cola hasta que
//se trabe la tarea de teclas
#define TASK1_MSG_QTY      10
#define TASK1_MSG_LENGTH   (sizeof(keys_t))

int8_t  task1CbPool [cbPoolSpace(TASK1_MSG_QTY,TASK1_MSG_LENGTH)];
circularBuffer_t t1Cb;
queue_t task1Queue;
event_t ledEvent;

void task1Init(void)
{
   queueInit ( &task1Queue,&t1Cb,task1CbPool,TASK1_MSG_QTY,TASK1_MSG_LENGTH );
   eventInit(&ledEvent,1);
}
//------------------------------------------
uint32_t task1Pool[REASONABLE_STACK];

taskParams_t task1Params = {
   .name      = "task1 deltas",
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

uint32_t calcKeyDeltas(timing_t* t,keys_t* k)
{
   t->deltaT1 = k->fallTime[1]-k->fallTime[0];
   t->deltaT2 = k->riseTime[1]-k->riseTime[0];
   t->sumT1T2 = abs(t->deltaT1)+abs(t->deltaT2);
   t->color   = (t->deltaT1<0?2:0) + (t->deltaT2<0);
   return t->sumT1T2;
}
void printKeyReport(timing_t* t,keys_t* keys)
{
   printUART("led %s encendido:\r\n"
         "\t tiempo encendido: %d ms\r\n"
         "\t tiempo entre flancos descendentes: %d ms\r\n"
         "\t tiempo entre flancos ascendentes : %d ms\r\n",
         colors_text[t->color],
         ticks2msec(t->sumT1T2),       //la suma ya calculada
         ticks2msec(abs(t->deltaT1)),  //piden valor absoluto
         ticks2msec(abs(t->deltaT2))   //valor absoluto
         );
}

void* task1(void* a)
{
   keys_t   keys;
   timing_t timing;
   while(1) {
      queueRead      ( &task1Queue ,&keys             );    // leo por copia la nueva estructura de tiempo en keys
      if (calcKeyDeltas(&timing ,&keys)>0) {                 // calculo y verfico que el tiempo de encendido sea mayora a cero
         eventGive      ( &ledEvent   ,(void*)&timing ,1 ); // aviso pasandole el puntero a esta nueva estructura de tiempo a la tarea de leds
         printKeyReport ( &timing     ,&keys             ); // envio a imprimir el reporte mientras la tare de leds hace lo suyo
         semphrTake     ( &ledSemphr                     ); // espero a que la tarea de leds termine sus cosas y vuelvo a empezar
      }
   }
   return NULL;
}
