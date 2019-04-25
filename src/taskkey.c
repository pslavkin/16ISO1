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
void updateKeyState         ( keys_t* keys,uint8_t i );
void updateTec1AndTec2State ( keys_t* keys           );

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
   keys_t keys = {
      .state       = {1,1},
      .tec1AndTec2 = 1,
      .name        = {TEC1,TEC2}
   };
   while(1) {
      for(uint8_t i=0;i<KEYS_QTY;i++) {
         updateKeyState(&keys,i);
      }
      updateTec1AndTec2State ( &keys         ) ;
      taskDelay              ( msec2Ticks(10 ));
   }
}

void updateKeyState(keys_t* keys,uint8_t i)
{
   bool newState=gpioRead(keys->name[i]);
   if((newState^keys->state[i])==1) {
      keys->state[i]=newState;
      uint32_t actualTick=getTicks();
      if(newState==1)
         keys->riseTime[i]=actualTick;
      else
         keys->fallTime[i]=actualTick;
   }
}
void updateTec1AndTec2State(keys_t* keys)
{
   if(keys->tec1AndTec2==1) {
      if( (keys->state[0] || keys->state[1])==0)
         keys->tec1AndTec2=0;
   }
   else {
      if( (keys->state[0] && keys->state[1])==1) {
         keys->tec1AndTec2=1;
         queueWrite(&task1Queue,keys);
      }
   }
}

