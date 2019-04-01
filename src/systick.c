#include <stdint.h>
#include "string.h"
#include "systick.h"
#include "os.h"
#include "sapi.h"

uint32_t tick = 0;

void initSystick(void)
{
   SysTick_Config( SystemCoreClock /100); //ticks cada 10msegs
}
void SysTick_Handler(void)
{
   tick++;
   uint8_t i,j;

   runningContext->state=READY;

   for (i=MAX_PRIOR;i>0;) {
      i--;
      for(j=0;j<MAX_TASK;j++) {
         switch (taskList[i][j].state) {
            case WAITING:
               if(--taskList[i][j].sleepTicks == 0)
                  taskList[i][j].state = READY;
               break;
            default:
               break;
         }
      }
   }
   yield();
}
