#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "taskkernel.h"

taskContext    kernelContext;
uint32_t       taskKernelPool[16];  //en el 1er salto, el sp se pisa con el del main... que tiene 32k

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
   uint8_t i,j;

   for (i=MAX_PRIOR;i>0;) {
      i--;
      for(j=0;j<MAX_TASK;j++) {
         switch (taskList[i][j].state) {
            case READY:
               runningContext        = &taskList[i][j];
               runningContext->state = RUNNING;
               return NULL;
               break;
            default:
               break;
         }
      }
   }
}
void* hookKernel(void* p)
{
   return NULL;
}
