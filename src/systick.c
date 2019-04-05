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
void enableSystickIrq(void)
{
  SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
}
void disableSystickIrq(void)
{
  SysTick->CTRL  &= ~SysTick_CTRL_TICKINT_Msk;
}

void SysTick_Handler(void)
{
   uint8_t i,j;
   tick++;                                 // es la que lleva cuenta del uptime del sistema

   tasks.context->state=READY;             // la tarea que fue interrumpida pasa a READY, pero
                                           // es que nunca salio de ready, ya que no es
                                           // necesario decirle al kernel que una tarea esta
                                           // corriendo, ya que solo una corre en cada instante
                                           // en un uC de un solo nucleo.. y se cual es por el
                                           // tasks.context. pero bueno, lo dejo porque podria
                                           // usarse en el futuro.

   for (i=MAX_PRIOR;i>0;) {                // arranca buscando desde la lista de tareas de max_prior para abajo
      i--;                                 // a ver si hay alguna para pasar a ready
      for(j=0;j<MAX_TASK;j++) {      // solo recorre las tareas habilitadas en cada prioridad
         switch (tasks.list[i][j].state) { // podria ser un if, pero tengo otros planes...
            case WAITING:                  // cuando agota el contador pasa a READY
               if(tasks.list[i][j].sleepTicks > 0)
                  tasks.list[i][j].sleepTicks--;
               else
                  tasks.list[i][j].state = READY;
               break;
            default:
               break;
         }
      }
   }
   triggerPendSv();                        // listo, hora de llamar al kernel para que despache la tarea que toque
}
