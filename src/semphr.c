#include <stdint.h>
#include "string.h"
#include "os.h"
#include "taskkernel.h"
#include "sapi.h"
#include "pendsv.h"
#include "systick.h"
#include "semphr.h"

bool mutexInit ( semphr_t* m )
{
   m->locked=false;  //arranca desbloqueado
};
//----------------------------------------------------------------------
bool mutexLock   ( semphr_t* m ){
   if(m->locked==false) {           // facil, si nadie lo tiene...
      m->locked=true;               // lo tengo yo
   }
   else {
      tasks.context->state=BLOCKED; // ok, ocupado..me pongo en BLOCKED (no conundor con waiting)
      tasks.context->semphr=m;      // me guardo el puntero al semaforo que estoy esperando
      triggerPendSv();              // y aca libero el uC, de modo que a partir de ahora soy una tarea BLOQUEAD.. y ahora? quien podra rescatarme??
   }
};

bool mutexUnlock   ( semphr_t* m )
{
   int8_t i,j,k;
   m->locked=false;                                    // primero libero el mutex
   // recorro HASTA mi misma prioridad. porque nadie de menor prioridad que yo puede interrumpirme. De modo
   // que si yo tengo el mutex, solo alguien mas importante pudo haber intentado quitarmelo. y a esa tarea es
   // a la que se lo doy.
   for (i=(MAX_PRIOR-1);i>=tasks.context->prior;i--) {
      k=tasks.index[i];                            // auxiliar para recorrer desde la ultima posicion de cada lista de prioridades
      for(j=0;j<MAX_TASK;j++) {                    // barro todas las tareas del grupo de prioridad
         k=(k+1)%MAX_TASK;                         // incremento modulo MAX
         switch (tasks.list[i][k].state) {         // podria haber sido un if.. pero tengo otros planes
            case BLOCKED:                          // aja, encontre una..veamos si me esta esperando...
               if(tasks.list[i][k].semphr==m) {    // si! me estaba esperando, le abro la puerta
                  tasks.list[i][k].state  = READY; // aviso que esta taera pasa a ready
                  tasks.list[i][k].semphr = NULL;  // borro el puntero al semphr
                  taskYield();                     // y aca esta la magia.. yo cedo el uC, porque alguien mas importante que yo o a lo sumo igual me estaba esperando para entrar al banio..
                  return true;
               }
               break;
            default:
               break;
         }
      }
   }
   return false;                                   //por ahora no uso la salida de esta func.
};


