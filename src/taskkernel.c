#include <stdint.h>
#include "string.h"
#include "sapi.h"
#include "os.h"
#include "systick.h"
#include "taskkernel.h"
#include "taskidle.h"
#include "stat.h"

//la tarea kernel se mantiene fuera de tasks que es la lista de tareeas de
//usuario, pero tiene las mismas cualidades que el resto de las tareas, aunque
//algunas sobran, se unifica porque mejora la lectura del codigo y se
//aprovechan todas las funciones
taskContext_t  kernelContext;

//en el 1er salto, el sp se pisa con el del main... que tiene 32k, por eso solo
//uso 16 bytes para el 1er llenado de datos al inicio.
uint32_t       taskKernelPool[ MIN_STACK4FILL];

void taskKernelBegin(void)
{
   kernelContext.waterMark = 0x7FFFFFFF;        //como el kernel despues se queda con el stack del main, le pongo un wtaremark gigante para que a la primera se ajuste al valor correcto
}

taskParams_t taskKernelParams = {
   .name     = "taskKernel",
   .pool     = taskKernelPool,
   .poolSize = sizeof(taskKernelPool)/sizeof(taskKernelPool[0]),
   .param    = NULL,
   .func     = taskKernel,
   .hook     = defaultHook,
   .begin    = taskKernelBegin,
   .end      = rien,
};

//esta es la tarea que se llama desde pendsv. tiene su propio stack como cualquier tarea, pero
//puede acceder a tasks y elegir la proxima tarea a ejecutar.
//se implemento SCHED_FIFO y SCHED_RR.
//En SCHED_FIFO si una tarea de cierta prioridad no libera el uC con taskDelay
//o taskYield, no da lugar a sus vecinas de ejecutarse (solo las de mas alta
//priodidad.
//En SCHED_RR es similar solo que aunque una tarea intente apropiarse del uC,el
//kernel la expropia y pregunta por la siguiente en su propia lista de
//prioridades dendole la vuelta a todo su grupo antes de que le toque
//nuevamente. De modo que es interrumpible por tareas de mas alta prioridad o
//igual prioridad
//la seleccion de un metodo u otro se hace con un define y solo cambia la posicion del
//incremento de la tarea en curso de cada prioridad como se ve en la siguiente funcion
void* taskKernel(void* p)
{
   int8_t i,j;                                                 // uso indices signados porque voy a comparar con >=0. se podria hacer tambien de otra manera
   disableSystickIrq();                                        // como systick husmea en mi lista de tareas, le pido que espere a que termine antes de que salte
#if STAT_ENABLE==1                                             // si es RR, PRIMERO incremento, con lo cual la que fue interrumpida pasa a ser la ultima en la busqueda rounrobin
   stampWaterMark ( tasks.context  );                          // funciones de estadisticas, utlies para debug
   stampRunCount  ( tasks.context  );
   stampWaterMark ( &kernelContext );
   stampRunCount  ( &kernelContext );
#endif
   for (i=(MAX_PRIOR-1);i>=0;i--) {                            // arranca siempre desde la maxima prioridad
      for(j=0;j<MAX_TASK;j++) {                                // barro todas las tareas del grupo de prioridad
#if SCHED_FIFO_RR==0                                           // si es RR, PRIMERO incremento, con lo cual la que fue interrumpida pasa a ser la ultima en la busqueda rounrobin
         tasks.index[i]=(tasks.index[i]+1)%MAX_TASK;           // incremento modulo MAX_TASK
#endif
         switch (tasks.list[i][tasks.index[i]].state) {        // podria hacer un if pero tego otros planes luego
            case READY:                                        // la primera que este READY
               tasks.context = &tasks.list[i][tasks.index[i]]; // actualizo el index
               goto end;                                       // en serio me lo decis?? sip. asi es. aca esta, prestando servicios, si Kernighan lo eligio yo lo uso.. Que queres que ponga flags en el for? bdddd
            default:
               break;
         }
#if SCHED_FIFO_RR==1                                           // si es FIFO, el incremento lo hago DESPUES de parsear, de modo que le permito a la tarea interrumpida volver a tomar el uC
         tasks.index[i]=(tasks.index[i]+1)%MAX_TASK;           // incremento modulo count
#endif
      }
   }
   tasks.context        = &idleContext;                        // ok, parece que esta tranquilo todo.. nada para hacer...
end:
   tasks.context->state = RUNNING;                             // esto es absolutamente evitable. lo dejo solo para mantener las formas
   enableSystickIrq();                                         // habilito d nuevo el systick
}
//-----------------------------------------------------------------------
bool freeBlockedGived   ( event_t* m)                // cuando se hace un take, libera las tareas esperando un give
{
   int8_t i,j,k;
   for (i=(MAX_PRIOR-1);i>=0;i--) {                  // recorro todas las prioridades pero SOLO hago yield si encuentro alguna tarea de mi misma o mayoyr que necesita el uC
      k=tasks.index[i];                              // auxiliar para recorrer desde la ultima posicion de cada lista de prioridades
      for(j=0;j<MAX_TASK;j++) {                      // barro todas las tareas del grupo de prioridad
         k=(k+1)%MAX_TASK;                           // incremento modulo MAX
         switch (tasks.list[i][k].state) {           // podria haber sido un if.. pero tengo otros planes
            case BLOCKED_GIVE:                       // aja, encontre una..veamos si me esta esperando...
               if(tasks.list[i][k].event==m) {       // si! me estaba esperando, le abro la puerta, si estaba bloqueada pero por otro semaforo, salteo
                  disableSystickIrq();               // voy a modificar el eventAns, si llegara a entrar tick ahora y justo le da tout habria condicion de carrera..
                  tasks.list[i][k].event    = NULL;  // TODO: no se si hace falta..borro el puntero al semphr
                  tasks.list[i][k].eventAns = true;  // perfecto, libero el recurso de manera correcta
                  tasks.list[i][k].state    = READY; // aviso que esta taera pasa a ready
                  enableSystickIrq();                // habilito de nuevo el systick
                  if(i>=tasks.context->prior)        // si la tarea que debloquie es de igual o mayor que la actual, tengo que hacer el yield! sino no. lo hace el kernel task luego, pero IGUAL la desbloqueo
                     return true;                    // aviso que voy a hacer yield, cuando termine de desbloquear todo
               }
               break;
            default:
               break;
         }
      }
   }
   return false;
}
bool freeBlockedTaked   ( event_t* m)                   // cuando se hace el give, libera las tareas esperando un take
{
   bool yield=false;
   int8_t i,j,k;
   for (i=(MAX_PRIOR-1);m->count>0 && i>=0;i--) {       // recorro todas las prioridades pero SOLO hago yield si encuentro alguna tarea de mi misma o mayoyr que necesita el uC
      k=tasks.index[i];                                 // auxiliar para recorrer desde la ultima posicion de cada lista de prioridades
      for(j=0;m->count>0 && j<MAX_TASK;j++) {           // barro todas las tareas del grupo de prioridad
         k=(k+1)%MAX_TASK;                              // incremento modulo MAX
         switch (tasks.list[i][k].state) {              // podria haber sido un if.. pero tengo otros planes
            case BLOCKED_TAKE:                          // aja, encontre una..veamos si me esta esperando...
               if(tasks.list[i][k].event==m) {          // si! me estaba esperando, le abro la puerta, si estaba bloqueada pero por otro semaforo, salteo
                  disableSystickIrq();                  // voy a modificar el eventAns, si llegara a entrar tick ahora y justo le da tout habria condicion de carrera..
                  m->count--;
                  tasks.list[i][k].event     = NULL;    // TODO: no se si hace falta..borro el puntero al semphr
                  tasks.list[i][k].eventData = m->data; // libero de manera correcta el recurso
                  tasks.list[i][k].eventAns  = true;    // libero de manera correcta el recurso
                  tasks.list[i][k].state     = READY;   // aviso que esta taera pasa a ready
                  enableSystickIrq();                   // habilito de nuevo el systick
                  if(i>=tasks.context->prior)           // si la tarea que debloquie es de igual o mayor que la actual, tengo que hacer el yield! sino no. lo hace el kernel task luego, pero IGUAL la desbloqueo
                     yield = true;                      // aviso que voy a hacer yield, cuando termine de desbloquear todo
               }
               break;
            default:
               break;
         }
      }
   }
   return yield;                                        // por ahora no uso la salida de esta func.
}
