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
#include "taskprint.h"
#include "stat.h"
#include "convert.h"

uint32_t task3Pool[REASONABLE_STACK];

taskParams_t task3Params = {
   .name      = "task3",
   .pool      = task3Pool,
   .poolSize  = sizeof(task3Pool)/sizeof(task3Pool[0]),
   .param     = NULL,
   .func      = task3,
   .hook      = defaultHook,
   .init      = NULL,
};

//no la estoy usando por ahora
void* task3(void* a)
{
   while(1) {
      taskDelay(msec2Ticks(1000));
      gpioToggle ( LED2 );
   }
   return NULL;
}
