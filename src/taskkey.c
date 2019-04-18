#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "mutex.h"
#include "semphr.h"
#include "queue.h"
#include "systick.h"
#include "taskkey.h"
#include "task1.h"
#include "taskprint.h"
#include "stat.h"

//------------------------------------------
uint32_t taskKeyPool[MIN_STACK];

taskParams_t taskKeyParams = {
   .name      = "taskKey",
   .pool      = taskKeyPool,
   .pool_size = sizeof(taskKeyPool)/sizeof(taskKeyPool[0]),
   .param     = NULL,
   .func      = taskKey,
   .hook      = defaultHook,
};

void* taskKey(void* a)
{
   while(1) {
      while(gpioRead(TEC1)==true)
         taskDelay(msec2Ticks(40));
      gpioToggle ( LED1                   ) ;
      queueWrite ( &printQueue,"Tec1\r\n" ) ;
      taskDelay ( msec2Ticks(500 ));
   }
   return NULL;
}
