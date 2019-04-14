#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "mutex.h"
#include "semphr.h"
#include "systick.h"
#include "task1.h"
#include "task3.h"
#include "taskprint.h"
#include "stat.h"

uint32_t task3Pool[MIN_STACK];

taskParams_t task3Params = {
   .name      = "task3",
   .pool      = task3Pool,
   .pool_size = sizeof(task3Pool)/sizeof(task3Pool[0]),
   .param     = NULL,
   .func      = task3,
   .hook      = defaultHook,
};

void* task3(void* a)
{
   while(1) {
      semphrTake ( &printfSemphr ); // con este mutex me evito que si otra
      gpioToggle ( LED3          );
   }
   return NULL;
}
