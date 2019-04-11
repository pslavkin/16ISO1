#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "semphr.h"
#include "systick.h"
#include "task1.h"

//------------------------------------------
//defino por ahora en esta tarea mi primer mutex que lo comparto con el resto
//de las tareas haciendolo ectern en el .h, pero el lugar correcto para
//definirlo seria en algun file relacionado con la transmision por uart
semphr_t printfMutex;
semphr_t printfSemphr;

void initPrintfMutex(void)
{
   mutexInit(&printfMutex);
}
void initPrintfSemphr(void)
{
   semphrInit(&printfSemphr);
}
//------------------------------------------
uint32_t task1Pool[MIN_STACK];

taskParams task1Params = {
   .name      = "task1",
   .pool      = task1Pool,
   .pool_size = sizeof(task1Pool)/sizeof(task1Pool[0]),
   .param     = NULL,
   .func      = task1,
   .hook      = defaultHook,
};

void* task1(void* a)
{
   uint32_t i,j;

   while(1) {
//pongo esta tarea a consumir uC sin parar para probar cuestiones de
//prioridades y que el scheduler le haga o no el preemptive y probara tambien
//lo de SCHED_RR y SCHED_FIFO
      for(j=0;j<100;j++) {
         for(i=0;i<1000000;i++)
            ;
         gpioToggle ( LED1         );
         mutexLock  ( &printfMutex );
            stdioPrintf(UART_USB,"Tarea= %s Numero= %d\r\n",
                  tasks.context->name,tasks.context->number);
         mutexUnlock ( &printfMutex );
      }
      taskDelay  ( mseg2Ticks(1000 ));
   }
   return NULL;
}
