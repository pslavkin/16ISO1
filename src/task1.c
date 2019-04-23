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
semphr_t task1Semphr;

void task1Init(void)
{
   semphrInit(&task1Semphr,5);
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

void* task1(void* a)
{
   while(1) {
      //semphrTake ( &task1Semphr                ) ;
      //gpioToggle ( LED1                         ) ;
      //taskCreate ( &taskTemplateParams ,5       ) ;   //crea una tarea en tiempo real
      //semphrTake ( &task1Semphr                ) ;
//      uartWriteString ( UART_USB    ,"1234" );
      if(semphrTakeTout ( &task1Semphr,msec2Ticks(8000))==false)
        queueWrite(&printQueue,"agoto task1\r\n");
      gpioToggle ( LED1                         ) ;
      taskDelay(msec2Ticks(500));
      //taskDelete ( taskFind(&taskTemplateParams ));   //la borra
   }
   return NULL;
}
