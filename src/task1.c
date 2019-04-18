#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "mutex.h"
#include "semphr.h"
#include "queue.h"
#include "systick.h"
#include "task1.h"
#include "tasktemplate.h"
#include "taskprint.h"
#include "stat.h"

//------------------------------------------
//defino por ahora en esta tarea mi primer mutex que lo comparto con el resto
//de las tareas haciendolo ectern en el .h, pero el lugar correcto para
//definirlo seria en algun file relacionado con la transmision por uart
semphr_t printfSemphr;

void initPrintfSemphr(void)
{
   semphrInit(&printfSemphr,5);
}
//------------------------------------------
uint32_t task1Pool[MIN_STACK];

taskParams_t task1Params = {
   .name      = "task1",
   .pool      = task1Pool,
   .pool_size = sizeof(task1Pool)/sizeof(task1Pool[0]),
   .param     = NULL,
   .func      = task1,
   .hook      = defaultHook,
};

void* task1(void* a)
{
   while(1) {
      semphrTake ( &printfSemphr                ) ;
      gpioToggle ( LED1                         ) ;
      taskCreate ( &taskTemplateParams ,5       ) ;
      semphrTake ( &printfSemphr                ) ;
      gpioToggle ( LED1                         ) ;
      taskDelete ( taskFind(&taskTemplateParams ));
   }
   return NULL;
}
