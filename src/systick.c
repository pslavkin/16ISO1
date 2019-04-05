#include <stdint.h>
#include "string.h"
#include "systick.h"
#include "os.h"
#include "sapi.h"

uint32_t tick = 0;     //aun no lo estoy usando pero es un contador de ticks multiuso

//agrego funciones para prender y apagar la irq del systick. Las uso antes de
//entrar al taskKernel para evitar race condition contra tasks.list. Es una
//opcion para no deshabilitar todas las irqs.. por ahora con eso me alcanza
void initSystick       ( void ) { SysTick_Config( SystemCoreClock /systickTimeDivisor)      ;} // ticks cada 10msegs
void enableSystickIrq  ( void ) { SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk ;}
void disableSystickIrq ( void ) { SysTick->CTRL  &= ~SysTick_CTRL_TICKINT_Msk;}

void SysTick_Handler(void)
{
   int8_t i,j;                                     // uso singado porque comparo con >=0 en el for. Se podria hacer de otra manera tambien
   tick++;                                         // es la que lleva cuenta del uptime del sistema

   tasks.context->state=READY;                     // la tarea que fue interrumpida pasa a READY, pero
                                                   // es que nunca salio de ready, ya que no es
                                                   // necesario decirle al kernel que una tarea esta
                                                   // corriendo, ya que solo una corre en cada instante
                                                   // en un uC de un solo nucleo.. y se cual es por el
                                                   // tasks.context. pero bueno, lo dejo porque podria
                                                   // usarse en el futuro.

   for (i=(MAX_PRIOR-1);i>=0;i--) {                // arranca buscando desde la lista de tareas de max_prior para abajo a ver si hay alguna para pasar a ready
      for(j=0;j<MAX_TASK;j++) {                    // recorre todas las tareas en cada prioridad
         switch (tasks.list[i][j].state) {         // podria ser un if, pero tengo otros planes...
            case WAITING:
               if(tasks.list[i][j].sleepTicks > 0) // comparo ANTES de descontar. oprque adminto un delay de cero
                  tasks.list[i][j].sleepTicks--;   // decremento y hasta la priximo
               else
                  tasks.list[i][j].state = READY;  // llego a cero! READY to work
               break;
            default:
               break;
         }
      }
   }
   triggerPendSv();                                // listo, hora de llamar al kernel para que despache la tarea que toque
}
