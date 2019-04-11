#include <stdint.h>
#include "string.h"
#include "os.h"
#include "taskkernel.h"
#include "sapi.h"
#include "taskidle.h"
#include "systick.h"

tasks_t  tasks; // aca defino la lista estatica de todas las tareas posibles de ser agregadas al sistema
                // es bastatnte ineficiente porque tengo que dejar previsto en compilacion las
                // que pienso usar, pero no me meto con mem dinamica y me concentro en el
                // resto del temario del curso. Para ser mas efectivo se podria ir creando una
                // lista de tareas anidadas y cada casillero se crea on runtime y en vez de
                // recorrer un arreglo estatico habria que ir siguiendo la cadena anidada

//tomando como stack lo que llega en taskContext, esta funcion guarda alli los
//parametros de la tarea segun el funcionamiento del cortexM4 para que se
//parezca a una tarea en curso notar que no se pushea el LR von 0xFFFFFFF9 ya
//que en el taskSwitch se hace un doble cambio de contexto para no usar el
//stack de la tarea interrumpida para guardar locales del taskKernel. Con lo
//cual en el taskSwitch se hace un doble cambio de contexto, primero se cambia
//de la runningTask a kernelTask y luego de kernelTask a la tarea elegida.  de
//esta manera se usa el stack de cada quien para cada cual.
bool pushTask(taskParams* t, taskContext* c)
{
   *(--c->sp)=1<<24              ; // PSR modo thumb
   *(--c->sp)=(uint32_t)t->func  ; // PC  funcion que 'esta' corriendo
   *(--c->sp)=(uint32_t)t->hook  ; // LR  en caso de que la funcion retorne, viene aca
   *(--c->sp)=0                  ; // r12 no es importnte su valor

   *(--c->sp)=0                  ; // r3  no es importante su valor
   *(--c->sp)=0                  ; // r2  no es importante su valor
   *(--c->sp)=0                  ; // r1  no es importante su valor
   *(--c->sp)=(uint32_t)t->param ; // r0  se puede utilizar para pasar un parametro al 1er
                                   // llamado

   *(--c->sp)=0xFFFFFFF9         ; // lr

   *(--c->sp)=7                  ; // r11 no es importante su valor
   *(--c->sp)=5                  ; // r10 no es importante su valor
   *(--c->sp)=5                  ; // r9  no es importante su valor
   *(--c->sp)=4                  ; // r8  no es importante su valor

   *(--c->sp)=3                  ; // r7  no es importante su valor
   *(--c->sp)=2                  ; // r6  no es importante su valor
   *(--c->sp)=1                  ; // r5  no es importante su valor
   *(--c->sp)=0                  ; // r4  no es importante su valor

}

//carga en tasks que es la estructura en donde se guardan todas las tareas con
//los parametros que se pasan. el nombre, la prioridad, etc.
bool taskFill(taskParams* t, taskContext* c, uint32_t prior)
{
      c->pool       = t->pool;                  //puntero al stack que usara la tarea
      c->sp         = &t->pool[t->pool_size];   //pntero al FINAL del stack
      c->state      = READY;                    //arranca ready
      c->sleepTicks = 0;                        //sin timer
      c->prior      = prior;                    //defino la prioridad
      strcpy   ( c->name ,t->name );            // la tarea guard su nombre
      pushTask ( t       ,c       );            //ahora emula como su estuviera corriendo
      return true;                              //TODO por ahora siempre funciona.. peeero
}

bool taskCreate(taskParams* t, uint32_t prior)
{
   uint8_t i;
   for(i=0;i<MAX_TASK;i++)
      if(tasks.list[prior][i].state==EMPTY) {
         tasks.list[prior][i].number=i;
         return taskFill(t,&tasks.list[prior][i],prior);
      }
   return false;
}
bool taskDelete(taskContext* c)
{
   c->state=EMPTY;      //se entiende? o lo tengo que explicar??
   return true;
}

//inicializo toda la lista de tareas para que esten todas en blanco, inicializo
//los contadores de tareas, los estados etc.
bool initTasks(void)
{
   uint8_t i,j;

   for(i=0;i<MAX_PRIOR;i++) {
      for(j=0;j<MAX_TASK;j++) {
         tasks.list[i][j].state=EMPTY; // no lo estoy usando, pero quede bien indicar que el
                                       // casillero esta vacio
      }
      tasks.index[i] = 0;              // arranco desde la 1er tarea de cada priodidadd
   }
  // si, el kernel tambien es una tarea, aunque no esta en la lista de tareas  de tasks..
  // tiene su estructura aparte, pero los mismos parametros.
   taskFill(&taskKernelParams,&kernelContext,0);
  // wah? el primer salto se hace desde main con el stack principal.. se lo regalo al
  // kerneltask por ahora, o sea en el primer salto de pendSV el sp del main se copiara al
  // taskKernel. Es por eso tambien que en la creacion del taskKernel se le asignan solo
  // 16bytes de stack para iniciar, porque no necesita ni un byte mas. ni bien arranque todo se
  // le asignaran los 32kb del main (o lo que sea que este configurado en el lkf
   tasks.context = &kernelContext;
  // tarea idle, fuera de la sabana de tareas, se ejecuta cuando no hay otra tarea en ready. 
   taskFill(&taskIdleParams,&idleContext,0);
}
void triggerPendSv(void)
{
   __ISB();
   __DSB();
   SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; // con este flag, saltra la irq pendSV inmediatamente
}
bool taskDelay(uint32_t t)
{
   tasks.context->sleepTicks = t;       // guardo los tiks. Cuando llegue a cero pasa a READY
   tasks.context->state      = WAITING; // o sea si guardo 2, hace tick->1 tick->0 tick->run
   triggerPendSv();                     // listo, llamo a cambio de contecto
}
bool taskYield(void)
{
   tasks.context->state = READY;        // la tarea que llama claramente esta en running, pasa a ready
   triggerPendSv();                     // listo, llamo a cambio de contecto
}

WEAK void* defaultHook(void* p)
{
   while(1)
      ;
};
