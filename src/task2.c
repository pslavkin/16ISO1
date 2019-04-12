#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "semphr.h"
#include "systick.h"
#include "task1.h"
#include "task2.h"

//es el stack de la tarea2. lo defino en base a los requisitos de lo que se
//pretende hacer. En particular ahora estoy uando printf, asi que ojo porque
//usa bastante stack. estoy probando con 300. cuando pueda medir el nivel de
//stack lo podre ajustar
uint32_t task2Pool[MIN_STACK];
//reuno todos los datos de la tarea para pasarlo a initTask asi no tengo que
//pasar tantos parametros y queda mas ordenado y limpio el codigo dejo fuera la
//prioridad, porque eso lo guardo en el os y ademas es si lo paso como
//paramertro e initTask para que se aprecie mejor al momento de inicializar las
//tareas en main
taskParams task2Params = {
   .name      = "task2",
   .pool      = task2Pool,
   .pool_size = sizeof(task2Pool)/sizeof(task2Pool[0]),
   .param     = NULL,
   .func      = task2,
   .hook      = defaultHook,
};

void* task2(void* a)
{
   float  pi=300.14;
   int p;
   while(1) {
      taskDelay(mseg2Ticks(20));
      gpioToggle(LED2);
      pi/=2;
      if(pi>10) p=100;
      else p=200;
      semphrTake ( &printfSemphr );                             // con este mutex me evito que si otra
      mutexLock  ( &printfMutex  );                             // con este mutex me evito que si otra
         stdioPrintf(UART_USB,"Tarea= %s Numero= %d pi=%d\r\n", // tarea estaba transmitiendo una trama,
            tasks.context->name,tasks.context->number, p);      // la corte.. sino que espero a que liberen
      mutexUnlock ( &printfMutex );                             // el mutex y aho lo tomo
   }
   return NULL;
}
