#include <stdint.h>
#include "string.h"
#include "os.h"
#include "taskkernel.h"
#include "sapi.h"
#include "pendsv.h"
#include "systick.h"
#include "semphr.h"

bool semphrInit ( semphr_t* s, uint32_t count )
{
   s->count = 0;     // arranca tomado, a diferencia del mutex.
   s->max   = count; // cantidad maxima de gives.. 
};
//----------------------------------------------------------------------
bool semphrTake   ( semphr_t* m ){
   if(m->count>0) {                 // facil, si nadie lo tiene...
     m->count--;                    // lo tengo yo, con 1 unidad
   }
   else {
      tasks.context->state=BLOCKED; // ok, ocupado..me pongo en BLOCKED (no conundor con waiting)
      tasks.context->semphr=m;      // me guardo el puntero al semaforo que estoy esperando
      triggerPendSv();              // y aca libero el uC, de modo que a partir de ahora soy una tarea BLOQUEAD.. y ahora? quien podra rescatarme??
   }
};

bool semphrGive   ( semphr_t* m , uint32_t qty)
{
   int8_t i,j,k;
   if(m->count<m->max) {            //si hay al menos un lugar, arranco
      if((m->count+qty)<m->max)     //si puedo dar todos los que me pidio, lo hago
         m->count+=qty;
      else
         m->count=m->max;           //sino pongo lo maximo posible
      // recorro HASTA mi misma prioridad. porque nadie de menor prioridad que yo
      // puede interrumpirme. Si es menor y quiere el semaforo tendra que esperar al 
      // dispatcher para que lo consulte.
      for (i=(MAX_PRIOR-1);i>=tasks.context->prior;i--) {
         k=tasks.index[i];                            // auxiliar para recorrer desde la ultima posicion de cada lista de prioridades
         for(j=0;j<MAX_TASK;j++) {                    // barro todas las tareas del grupo de prioridad
            k=(k+1)%MAX_TASK;                         // incremento modulo MAX
            switch (tasks.list[i][k].state) {         // podria haber sido un if.. pero tengo otros planes
               case BLOCKED:                          // aja, encontre una..veamos si me esta esperando...
                  if(tasks.list[i][k].semphr==m) {    // si! me estaba esperando, le abro la puerta, si estaba bloqueada pero por otro semaforo, salteo
                     tasks.list[i][k].state  = READY; // aviso que esta taera pasa a ready
                     tasks.list[i][k].semphr = NULL;  // TODO: no se si hace falta..borro el puntero al semphr
                     m->count--;                      // no hace falta preguntar por mayor a cero porque si estoy aca es porque al inicio incremente en uno el count 
                     taskYield();                     // y aca esta la magia.. YO cedo el uC,
                     // porque alguien mas importante que yo o a
                     // lo sumo igual me estaba esperando para
                     // entrar al banio..
                     return true;
                  }
                  break;
               default:
                  break;
            }
         }
      }
   }
   return false;                                   //por ahora no uso la salida de esta func.
};

//----------------------------------------------------------------------
bool mutexInit ( semphr_t* m )
{
   m->count = 1; // arranca en 1, que seria desbloqueado en 1 unidad
   m->max   = 1; //
};
bool mutexLock ( semphr_t* s )
{
   return semphrTake(s);
}
bool mutexUnlock ( semphr_t* s )
{
   return semphrGive(s,1);
};
