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

void* task3(void* a)
{
   while(1) {
      uint8_t* b;
      eventTake(&task2Event,(void*)&b);
//      queueWrite ( &printQueue,b);
   }
   return NULL;
}
