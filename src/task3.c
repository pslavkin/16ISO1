#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "mutex.h"
#include "semphr.h"
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
   float  f=3.14;
   uint8_t data[20];                // se necesitan 15 para el numero mas grande representable +2147483647.647
   while(1) {
      taskDelay(msec2Ticks(2000));
//      semphrTake ( &task2Semphr ); // con este mutex me evito que si otra
//      gpioToggle ( LED3          );
//      uint32_t i;
//      for(i=0;i<(sizeof(buf[0])/sizeof(buf[0][0]))-3;i++) {
//         buf[actualBuf][i]++;
//      }
//      f*=1.005;
//      queueWrite ( &printQueue,buf[actualBuf]  ) ;
//      queueWrite(&printQueue,ftostr(f,data));
   }
   return NULL;
}
