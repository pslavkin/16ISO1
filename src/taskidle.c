#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "taskidle.h"

uint32_t taskIdlePool[MIN_STACK];

taskParams taskIdleParams = {
   .name      = "taskIdle",
   .pool      = taskIdlePool,
   .pool_size = sizeof(taskIdlePool)/sizeof(taskIdlePool[0]),
   .param     = NULL,
   .func      = taskIdle,
   .hook      = hookIdle
};

void* taskIdle(void* a)
{
   while(1) {
      gpioToggle(LEDB); // solo toglea un led ara verlo en la placa y pone a dormir todo hasta
      __WFI();          // la proxima irq
}
   return NULL;
}
void* hookIdle(void* p)
{
}
