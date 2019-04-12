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
   uint32_t firstTick,diff;

   while(1) {
      if ( gpioRead( TEC1 )==0) {
         firstTick=getTicks();
         gpioWrite ( LED1,true    );
         mutexLock ( &printfMutex );
         stdioPrintf(UART_USB,"Tarea= %s Numero= %d Boton Tec1\r\n",
               tasks.context->name,tasks.context->number);
         mutexUnlock ( &printfMutex );
         while(gpioRead(TEC1)==0)
            taskDelay(mseg2Ticks(50));
         diff=deltaTick(firstTick);
         gpioWrite(LEDG,true);
         stdioPrintf(UART_USB,"delay=%d\r\n",diff);
         taskDelay(diff);
         gpioWrite(LEDG,false);
         
         semphrGive ( &printfSemphr );
      }
        else {
            taskDelay(mseg2Ticks(50));
        }

   }
   return NULL;
}
