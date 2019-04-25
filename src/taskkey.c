#include <stdint.h>
#include "os.h"
#include "sapi.h"
#include "queue.h"
#include "systick.h"
#include "taskkey.h"
#include "task1.h"

void updateKeyState         ( keys_t* keys,uint8_t i );
void updateTec1AndTec2State ( keys_t* keys           );
//-----------------------------------------------------------
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
   keys_t keys = {               //esta estructura la cargo en el stadk de la propia tarea
      .state       = {1,1},      //lleva el estado actual de las teclas
      .tec1AndTec2 = 1,          //estado de la combinatoria de Tec1 y Tec2
      .name        = {TEC1,TEC2} //nombres de sapi para manejar las entradas
   };
   while(1) {
      for(uint8_t i=0;i<KEYS_QTY;i++) {      //leo todas las teclas
         updateKeyState(&keys,i);            //actualizo su estado
      }
      updateTec1AndTec2State ( &keys         ) ;   //actualizo ahora el estao conjunto de tec1 y tec2
      taskDelay              ( msec2Ticks(10 ));   //pool cada 10mseg
   }
}

void updateKeyState(keys_t* keys,uint8_t i)
{
   bool newState=gpioRead(keys->name[i]);                     // leo el estado actuala
   if(newState != keys->state[i]) {                           // si cambio
      keys->state[i]=newState;                                // actualizo
      (newState?keys->riseTime:keys->fallTime)[i]=getTicks(); // cargo el tiempo para rise o fall
   }
}
void updateTec1AndTec2State(keys_t* keys)
{
   if(keys->tec1AndTec2==1) {                      //este es el estado actual, ambas en 1
      if( (keys->state[0] || keys->state[1])==0)   //si pasaron las 2 a cero
         keys->tec1AndTec2=0;                      //cambio de estado
   }
   else {
      if( (keys->state[0] && keys->state[1])==1) { //si estaba en cero u pasaron las 2 a 1, este es el momento de interes
         keys->tec1AndTec2=1;                      //cambio de estado
         queueWrite(&task1Queue,keys);             //paso por copia la lista de todos los tiempo, para procesar y esta tarea vuelve a procesar teclas
      }
   }
}
