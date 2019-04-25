#include <stdint.h>
#include "string.h"
#include "systick.h"
#include "os.h"
#include "taskprint.h"
#include "sapi.h"

uint32_t tick = 0;     //cuenta con cada tick irq indefinidamente y la uso para calcular tiempos

//agrego funciones para prender y apagar la irq del systick. Las uso antes de
//entrar al taskKernel para evitar race condition contra tasks.list. Es una
//opcion para no deshabilitar todas las irqs.. por ahora con eso me alcanza
void initSystick       ( void ) { SysTick_Config( SystemCoreClock /systickTimeDivisor)      ;} // ticks cada 10msegs
void enableSystickIrq  ( void ) { SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk ;}
void disableSystickIrq ( void ) { SysTick->CTRL  &= ~SysTick_CTRL_TICKINT_Msk;}

void SysTick_Handler(void)
{
   int8_t i,j;                                    // uso singado porque comparo con >=0 en el for. Se podria hacer de otra manera tambien
   tick++;                                        // es la que lleva cuenta del uptime del sistema

   tasks.context->state=READY;                    // la tarea que fue interrumpida pasa a READY, pero
                                                  // es que nunca salio de ready, ya que no es
                                                  // necesario decirle al kernel que una tarea esta
                                                  // corriendo, ya que solo una corre en cada instante
                                                  // en un uC de un solo nucleo.. y se cual es por el
                                                  // tasks.context. pero bueno, lo dejo porque podria
                                                  // usarse en el futuro.

   for (i=(MAX_PRIOR-1);i>=0;i--) {                     // arranca buscando desde la lista de tareas de max_prior para abajo a ver si hay alguna para pasar a ready
      for(j=0;j<MAX_TASK;j++) {                         // recorre todas las tareas en cada prioridad
         switch (tasks.list[i][j].state) {              // podria ser un if, pero tengo otros planes...
            case WAITING:
               if(tasks.list[i][j].sleep > 0)           // comparo ANTES de descontar. oprque adminto un delay de cero
                  tasks.list[i][j].sleep--;             // decremento y hasta la priximo
               else
                  tasks.list[i][j].state = READY;       // llego a cero! READY to work
               break;
            case BLOCKED_GIVE:                          // para los estados bloqueados agrego la opcion de que tengan un timeout tambien
            case BLOCKED_TAKE:
               if(tasks.list[i][j].eventTout==true) {   // uso un flag para determinar si pidieron o no usar timeout
                  if(tasks.list[i][j].sleep > 0) {      // comparo ANTES de descontar. oprque adminto un delay de cero
                     tasks.list[i][j].sleep--;          // decremento y hasta la priximo
                  }
                  else {
                     tasks.list[i][j].state    = READY; // llego a cero! READY to work
                     tasks.list[i][j].event    = NULL;  // TODO: no se si hace falta..borro el puntero al semphr
                     tasks.list[i][j].eventAns = false; // chan.. salio por timeout la tarea bloqueada, espero que el que llamo pregunte por este campo porque sino leera abasura..
                  }
               }
               break;
            default:
               break;
         }
      }
   }
   triggerPendSv();                                // listo, hora de llamar al kernel para que despache la tarea que toque
}
//---------------------------------------------------------------------------------------
uint32_t getTicks(void)
{
   return tick;                        // no mucho para decir, me devuelve la cantdad de ticks que cuenta con el sistick
}
uint32_t diffTicks(uint32_t first, uint32_t second)
{
   uint32_t diff;                      // dificilmente pegue una vuelta, pero por las dudas controlo y defuelvo la diff
   if(second>first)
      diff=second-first;
   else
      diff=(0xFFFFFFFF-first)+second+1;   //aja... justo second pego la vuelta pero first no.. corrijo
   return diff;
}
uint32_t deltaTick(uint32_t first)
{
   return diffTicks(first,getTicks()); // le paso un tiempo inicial y me da la diferencia con el tiempo actual
}

void printUpTime(void)
{
   uint32_t hh;
   uint8_t  mm,ss;
   uint16_t msec;
   uint32_t total=ticks2msec(getTicks());
   hh     = total/ 3600000;
   total -= hh*    3600000;
   mm     = total/ 60000;
   total -= mm*    60000;
   ss     = total/ 1000;
   total -= ss*    1000;
   msec   = total;
   printUART("%04d:%02d:%02d:%03d\r\n",hh,mm,ss,msec);
}
