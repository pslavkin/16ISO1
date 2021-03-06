#include <stdint.h>
#include "string.h"
#include "event.h"
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
bool pushTask(taskParams_t* t, taskContext_t* c)
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
bool taskFill(taskParams_t* t, taskContext_t* c, uint32_t prior)
{
      c->pool      = t->pool;                // puntero al stack que usara la tarea, se usa solo para calcular el watermark y estadisticas, pero nada mas
      c->poolSize  = t->poolSize;            // lo uso solo para fines de stat.. pero no se usa para otra cosa.
      c->sp        = &t->pool[t->poolSize];  // pntero al FINAL del stack
      c->state     = READY;                  // arranca ready
      c->sleep     = 0;                      // sin timer
      c->event     = NULL;                   // no hace falta, pero por prolijidad
      c->eventData = NULL;                   // no hace falta, pero por prolijidad
      c->eventAns  = false;                  // no hace falta, pero por prolijidad
      c->eventTout = false;                  // no hace falta, pero por prolijidad
      c->prior     = prior;                  // defino la prioridad
      c->runCount  = 0;                      // numero de veces que corrio.
      strcpy   ( c->name ,t->name );         // la tarea guard su nombre
      pushTask ( t       ,c       );         // ahora emula como su estuviera corriendo
      c->waterMark = c->sp-c->pool;          // ahora que meti algo en el stack, defino el watermark
      t->begin();           //si cargo alguna tarea de init, se lanza ahora. Este init esta pensado para hacer tareas de inicializacion ANTES de que arranque el scheduler, cosa de que todo este listo para toas las tareas antes de arrancar, porque si un recurso de una tarea es utilizado por la otra y al arrancar le toca primero a la ultima, podrian haber cosas sin inisializar.. sino tengo que inicializarlos en el main o algo.. mejor asi..
      return true;                           // TODO por ahora siempre funciona.. peeero
}

//busca un huevo en la lista de la prioridad solicitada y si no hay ninguna empty 
//prueba si no hay alguna deleted, que lo uso para debug para dejar rastros de tareas borradas
//on the fly...
bool taskCreate(taskParams_t* t, uint32_t prior)
{
   uint8_t i;
   //en estado empty significa que nunca fue usado.. si lo encuentro lo tomo
   for(i=0;i<MAX_TASK && tasks.list[prior][i].state!=EMPTY;i++)
      ;
   //sin no hay en empty busco en delete, que es un lugar que no esta usado, pero que fue usado
   //alguna vez, estoy usando DELETED solo para debug, para ver como se borra en la grilla de
   //stat, pero tranquilamente se puede volar de todo el sistema
   if(i==MAX_TASK)
      for(i=0;i<MAX_TASK && tasks.list[prior][i].state!=DELETED;i++)
         ;
   if(i==MAX_TASK)
      return false;                                   // todo lleno.. vuelva mas tarde
   else {                                             // encontro, cargo la tarea
      tasks.list[prior][i].number=i;
      return taskFill(t,&tasks.list[prior][i],prior); // taskfill tambien podria fallar porque podria no contar con memoria, o algo, por ahora siempre anda
   }
}

//busca por nombre nua tarea en la lista.. no es tan eficiente, pero no estoy guardando un ID
//unico por tarea.. sera mas interesante, o sea, el numero esta es la sima de prior|index pero
//no lo estoy gardando en ninguna ladao por ahora...
taskContext_t* taskFind(taskParams_t* t)
{
   uint8_t i,j;
   for(i=0;i<MAX_PRIOR;i++)
      for(j=0;j<MAX_TASK;j++) {
         if(tasks.list[i][j].state!= EMPTY            &&
            tasks.list[i][j].state!= DELETED          &&
            strcmp(tasks.list[i][j].name,t->name)==0)
               return &tasks.list[i][j];
      }
   return NULL;
}
bool taskdelete4params(taskParams_t* p)
{
   taskContext_t* c=taskFind(p);
   if(c!=NULL) {        // si me piden borrar una tarea null, explota!
      p->end();         // se llama a la funcion de fin de tarea
      c->state=DELETED; // se entiende? o lo tengo que explicar??
   }
   return true;
}
bool taskDelete(taskContext_t* c)
{
   if(c!=NULL) {        // si me piden borrar una tarea null, explota!
      c->state=DELETED; // se entiende? o lo tengo que explicar??
   }
   return true;
}

//inicializo toda la lista de tareas para que esten todas en blanco, inicializo
//los contadores de tareas, los estados etc.
bool initTasks(void)
{
   uint8_t i,j;

   for(i=0;i<MAX_PRIOR;i++) {
      for(j=0;j<MAX_TASK;j++) {
         tasks.list[i][j].state = EMPTY; // arranca en EMPTY que se interpreta como hueco en la lista de tareas, se puede usar para meter una tarea on run-time
      }
      tasks.index[i] = 0;                // arranco desde la 1er tarea de cada priodidadd
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
void triggerPendSv4Isr(void)
{
   tasks.context->state=READY;   //como interrumpi una tarea que estaba en running, la paso a ready, ya que no tiene porque frenarse.. veremos si el uC esta de acuerdo, pero si lo esta, ya le tocare el turno nuevamente
   triggerPendSv();              //ahora si llamo a irq
}
void triggerPendSv(void)
{
   __ISB();
   __DSB();
   SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; // con este flag, saltra la irq pendSV inmediatamente
}
bool taskDelay(uint32_t t)
{
   tasks.context->sleep = t>0?(t-1):t;  // si le pido 1 tick, me hace 2 saltos de systick. en el primero detecta que es uno y lo pone en cero, y en el proximo detecta cero y avisa.. por eso resto uno y al mismo tiempo controlo que no reviente con cero.
   tasks.context->state = WAITING;      // o sea si guardo 2, hace tick->1 tick->0 tick->run
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
void rien(void)                         // funcion que no hace nada, me evito preguntar por NULL a cada rato, mejor llamo siempre y el que no tiene nada que hacer, RIEN
{
};

