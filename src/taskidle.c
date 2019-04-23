#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "taskidle.h"

uint32_t taskIdlePool[REASONABLE_STACK];
taskContext_t  idleContext; // la tarea Idle esta fuera de la lista estatica de
                            // tareas, con lo cual se le asigna este contexto
                            // sueltito para que use.  de ese modo la idle es
                            // la tarea mas baja de todas las prioridades, de
                            // hecho no se le pregunta la prioridad nunca, solo
                            // se ejecuta cuando ninguna otra esta ready y el
                            // scheduler no tiene lo que hacer...

taskParams_t taskIdleParams = {
   .name      = "taskIdle",
   .pool      = taskIdlePool,
   .poolSize  = sizeof(taskIdlePool)/sizeof(taskIdlePool[0]),
   .param     = NULL,
   .func      = taskIdle,
   .hook      = defaultHook,
   .init      = NULL,
};

WEAK void* taskIdle(void* a) // atributo weak para que pueda ser redefinida por el usuario sin tocar nada..
{
   while(1) {
      gpioToggle(LEDB); // solo toglea un led ara verlo en la placa y pone a dormir todo hasta
      __WFI();          // la proxima irq
   }
   return NULL;
}
