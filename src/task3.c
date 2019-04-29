#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "mutex.h"
#include "semphr.h"
#include "event.h"
#include "systick.h"
#include "task1.h"
#include "task2.h"
#include "task3.h"
#include "tasktemplate.h"
#include "taskprint.h"
#include "stat.h"
#include "convert.h"

uint32_t task3Pool[REASONABLE_STACK];
event_t task3Event;

void task3Begin(void)
{
   printUART("task3 begin\r\n");
   eventInit(&task3Event,10);
}
void task3End(void)
{
   printUART("task3 end\r\n");
}

taskParams_t task3Params = {
   .name     = "task3",
   .pool     = task3Pool,
   .poolSize = sizeof(task3Pool)/sizeof(task3Pool[0]),
   .param    = NULL,
   .func     = task3,
   .hook     = defaultHook,
   .begin    = task3Begin,
   .end      = task3End,
};

//zona de memoria compartida entre task 3 y tasktemplate, y task3 le manda el puntero a la
//estructura a traves de un evento, luego tasktemplate modifica el float de la estructura, y
//luego task3 cuando tasktempalste le devuelve el favor tambien
shared_t s={.f=123.456, .u=0, .b=true, .s="hola"};

void* task3(void* a)
{
   shared_t* s_task3=&s;
   while(1) {
      eventGive(&taskTemplateEvent,(void*)s_task3,1);
      printUART("task3 send shared struct pointer 2 taskTemplate\r\n");
      eventTake(&task3Event,(void*)&s_task3);
      s_task3->f*=2.01;
      if(s_task3->f>10000) s_task3->f=1;
      taskDelay(msec2Ticks(100));
   }
   return NULL;
}
