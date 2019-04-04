#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "taskkernel.h"

taskContext    kernelContext;      // la tarea kernel se mantiene fuera de
                                   // tasks que es la lista de tareeas de usuario, pero tiene
                                   // las mismas cualidades que el resto de las tareas, aunque
                                   // algunas sobran, se unifica porque mejora la lectura del
                                   // codigo y se aprovechan todas las funciones
uint32_t       taskKernelPool[16]; // en el 1er salto, el sp se pisa con el del
                                   // main... que tiene 32k, por eso solo uso 16 bytes para el
                                   // 1er llenado de datos

taskParams taskKernelParams = {
   .name      = "taskKernel",
   .pool      = taskKernelPool,
   .pool_size = sizeof(taskKernelPool)/sizeof(taskKernelPool[0]),
   .param     = NULL,
   .func      = taskKernel,
   .hook      = hookKernel
};

//esta es la tarea que se llama desde pendsv. tiene su propio stack como cualquier tarea, pero
//puede acceder a tasks y elegir la proxima tarea a ejecutar.
//por el momento implemento SCHED_FIFO en donde si una tarea de cierta prioridad no libera el
//uC con taskDelay o taskYield, no da lugar a sus vecinas de ejecutarse (solo las de mas alta
//priodidad. 
//Tambien implementa SCHED_RR que es similar solo que aunque una tarea intente apropiarse del
//uC,el kernel la expropia y pregunta por la siguiente en su lista de prioridades dendole la
//vuelta a todo su grupo antes de que le toque nuevamente. 
//la seleccion de un metodo u otro se hace con un define y solo cambia la posicion del
//incrementeo de la tarea en curso de cada prioridad como se ve en la siguiente funcion
void* taskKernel(void* p)
{
   uint8_t i,j;

   for (i=MAX_PRIOR;i>0;) {                                           // arranca siempre desde la maxima prioridad
      i--;
      for(j=0;j<tasks.count[i];j++) {                                 // barro todas las tareas del grupo de prioridad
#if SCHED_FIFO_RR==0                                                  // si es RR, PRIMERO incremento
         tasks.index[i]=(tasks.index[i]+1)%tasks.count[i];            // incremento modulo count
#endif
         switch (tasks.list[i][tasks.index[i]].state) {               // podria hacer un if pero tego otros planes luego
            case READY:                                               // la primera que este READY
               tasks.context        = &tasks.list[i][tasks.index[i]]; // actualizo el index
               tasks.context->state = RUNNING;                        // cambio el estado que ni siquiera es necesario por el momento
               return NULL;                                           // sin mas preambulos, vuelvo a la irq para cambiar el contecto usando tass.context como nueva tarea
               break;
            default:
               break;
         }
#if SCHED_FIFO_RR==1                                                  // si es FIFO, el incremento lo hago DESPUES de parsear, de modo que le permito a la tarea interrumpida volver a tomar el uC
         tasks.index[i]=(tasks.index[i]+1)%tasks.count[i];
#endif
      }
   }
}
void* hookKernel(void* p)
{
   return NULL;
}
