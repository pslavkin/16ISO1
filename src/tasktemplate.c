#include <stdint.h>
#include "os.h"
#include "sapi.h"
#include "semphr.h"
#include "queue.h"
#include "mutex.h"
#include "systick.h"
#include "tasktemplate.h"

//es el stack de la tarea2. lo defino en base a los requisitos de lo que se pretende hacer. 
uint32_t taskTemplatePool[MIN_STACK];

//reuno todos los datos de la tarea para pasarlo a initTask asi no tengo que
//pasar tantos parametros y queda mas ordenado y limpio el codigo dejo fuera la
//prioridad, porque eso lo guardo en el os y ademas es si lo paso como
//paramertro e initTask para que se aprecie mejor al momento de inicializar las
//tareas en main
taskParams_t taskTemplateParams = {
   .name      = "taskTemplate",
   .pool      = taskTemplatePool,
   .pool_size = sizeof(taskTemplatePool)/sizeof(taskTemplatePool[0]),
   .param     = NULL,
   .func      = taskTemplate,
   .hook      = defaultHook,
};

void* taskTemplate(void* a)
{
   while(1) {
      taskDelay  ( mseg2Ticks(1000 ));
      gpioToggle ( LED3            ) ;
   }
   return NULL;
}