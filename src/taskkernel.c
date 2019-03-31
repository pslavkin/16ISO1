#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "taskkernel.h"

uint32_t taskKernelPool[MIN_STACK];
taskContext    *runningContext;

taskParams taskKernelParams = {
   .name      = "taskKernel",
   .pool      = taskKernelPool,
   .pool_size = sizeof(taskKernelPool)/sizeof(taskKernelPool[0]),
   .param     = NULL,
   .func      = taskKernel,
   .hook      = hookKernel
};

void* taskKernel(void* p) //round robin por ahora (pero con hook)
{
   uint8_t i;
   uint8_t actualPrior = 0;
   uint8_t runningTask;

   for(i=1;i<MAX_TASK;i++) {
      switch (taskList[i].state) {
         case SLEEPING:
            if(--taskList[i].sleepTicks == 0)
               taskList[i].state = ACTIVE;
            break;
         case ACTIVE:
            if(taskList[i].prior >= actualPrior) {
               actualPrior = taskList[i].prior;
               runningTask = i;
            }
            break;
         default:
            break;
      }
      runningContext=&taskList[runningTask];
   }
}
void* hookKernel(void* p)
{
   return NULL;
}
