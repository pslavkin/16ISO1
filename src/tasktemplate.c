#include <stdint.h>
#include "os.h"
#include "sapi.h"
#include "semphr.h"
#include "queue.h"
#include "mutex.h"
#include "systick.h"
#include "taskprint.h"
#include "task3.h"
#include "tasktemplate.h"
#include "convert.h"

//es el stack de la tarea. lo defino en base a los requisitos de lo que se pretende hacer. 
uint32_t taskTemplatePool[REASONABLE_STACK];
event_t taskTemplateEvent;

void taskTemplateBegin(void)
{
   eventInit(&taskTemplateEvent,10);
}
//reuno todos los datos de la tarea para pasarlo a initTask asi no tengo que
//pasar tantos parametros y queda mas ordenado y limpio el codigo dejo fuera la
//prioridad, porque eso lo guardo en el os y ademas es si lo paso como
//paramertro e initTask para que se aprecie mejor al momento de inicializar las
//tareas en main
taskParams_t taskTemplateParams = {
   .name     = "taskTemplate",
   .pool     = taskTemplatePool,
   .poolSize = sizeof(taskTemplatePool)/sizeof(taskTemplatePool[0]),
   .param    = NULL,
   .func     = taskTemplate,
   .hook     = defaultHook,
   .begin    = taskTemplateBegin,
   .end      = rien,
};

//tarea blinky de debug
void* taskTemplate(void* a)
{
   shared_t* s_template;
   while(1) {
      if(eventTakeTout(&taskTemplateEvent,(void*)&s_template,1000)) {
         uint8_t buf[30];
         ftostr(s_template->f,buf);
         printUART("task template receive struct pointer uint=%d, bool=%d, msg=%s, float=%s",
               s_template->u,s_template->b,s_template->s,buf);
         s_template->f/=2;
         s_template->u++;
         eventGive(&task3Event,(void*)s_template,1);
      }
      else
      gpioToggle ( LED2 );
   }
   return NULL;
}
