#include <stdint.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "mutex.h"
#include "semphr.h"
#include "queue.h"
#include "systick.h"
#include "taskkey.h"
#include "task1.h"
#include "taskprint.h"
#include "stat.h"

//------------------------------------------
uint32_t taskKeyPool[REASONABLE_STACK];

taskParams_t taskKeyParams = {
   .name      = "taskKey",
   .pool      = taskKeyPool,
   .poolSize  = sizeof(taskKeyPool)/sizeof(taskKeyPool[0]),
   .param     = NULL,
   .func      = taskKey,
   .hook      = defaultHook,
   .init      = NULL,
};

void* taskKey(void* a)
{
   uint8_t i;
   struct keys_struct{
      uint8_t  state;
      gpioMap_t name[4];
   } keys= {.state=0x00, .name={TEC1,TEC2,TEC3,TEC4}};

   while(1) {
      //barro las 4 teclas y junto el estado en un byte para poder hacer un switch y asi
      //detectar pusaciones concurrentes y demas cosas mas practicas de hacer si todas las
      //teclas estan actualizadas en un solo byte
      for(i=0;i<sizeof(keys.name)/sizeof(keys.name[0]);i++) {
         if(gpioRead(keys.name[i]))
            keys.state &= ~(1<<i);  //aprovecho a invertir el estado.
         else                       //1 representa apretado, y cero libreado
            keys.state |= (1<<i);
      }
      switch  (keys.state) {
         case 0x00:
            break;
         case 0x01:
            queueWrite ( &printQueue,"Tec1\r\n" ) ;
            break;
         case 0x02:
            queueWrite ( &printQueue,"Tec2\r\n" ) ;
            break;
         case 0x04:
            queueWrite ( &printQueue,"Tec3\r\n" ) ;
            break;
         case 0x08:
            queueWrite ( &printQueue,"Tec4\r\n" ) ;
            break;
         default:
            queueWrite ( &printQueue,"invalid Tec\r\n" ) ;
            break;
      }
      taskDelay(msec2Ticks(40));
   }
   return NULL;
}
