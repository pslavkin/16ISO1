#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "taskkernel.h"

taskContext    kernelContext;
uint32_t       taskKernelPool[16];  //en el 1er salto, el sp se pisa con el del main... que tiene 32k
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

   for(i=0;i<MAX_TASK;i++) {
      switch (taskList[i].state) {
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
