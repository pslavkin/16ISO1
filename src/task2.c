#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "semphr.h"
#include "event.h"
#include "mutex.h"
#include "systick.h"
#include "task1.h"
#include "task2.h"
#include "taskprint.h"
#include "stat.h"
#include "convert.h"

uint32_t task2Pool[REASONABLE_STACK];

void task2Init(void)
{
}

taskParams_t task2Params = {
   .name      = "task2",
   .pool      = task2Pool,
   .poolSize  = sizeof(task2Pool)/sizeof(task2Pool[0]),
   .param     = NULL,
   .func      = task2,
   .hook      = defaultHook,
   .init      = task2Init,
};

void setLedOff(void)
{
   gpioWrite(LEDG,0);
   gpioWrite(LEDR,0);
   gpioWrite(LED1,0);
   gpioWrite(LEDB,0);
}
void setLed(uint8_t color)
{
   setLedOff();
   switch(color) {
      case 0:
         gpioWrite(LEDG,1);
         break;
      case 1:
         gpioWrite(LEDR,1);
         break;
      case 2:
         gpioWrite(LED1,1);
         break;
      case 3:
         gpioWrite(LEDB,1);
         break;
      default:
         break;
   }
}

void* task2(void* a)
{
   timing_t* t;
   while(1) {
      eventTake(&ledEvent,(void*)&t);
      setLed    ( t->color   );
      taskDelay ( t->sumT1T2 );
      setLedOff (            );
   }
   return NULL;
}
