#include <stdint.h>
#include "os.h"
#include "sapi.h"
#include "queue.h"
#include "systick.h"
#include "taskkey.h"
#include "task1.h"
#include "taskprint.h"

//TODO: hay que deshabilitar la irq definida en el archivo del sensor oultrasonico
#define OVERRIDE_SAPI_HCSR04_GPIO_IRQ

event_t keyEvent;

void updateKeyState         ( keys_t* keys,uint8_t i                                 );
void updateTec1AndTec2State ( keys_t* keys                                           );
void Set_Irq_Tec            ( uint8_t Irq_Ch,uint8_t Port, uint8_t Pin, Edges_T Edge );
void printKeyStruct         ( keys_t* keys                                                    );
//-----------------------------------------------------------
uint32_t taskKeyPool[REASONABLE_STACK];

void taskKeyBegin(void)
{
   eventInit(&keyEvent,10);   //hasta 10 presionadas de tecla sin que se atiendan, dificilmente se logre esto a mano
}

taskParams_t taskKeyParams = {
   .name      = "taskKey",
   .pool      = taskKeyPool,
   .poolSize  = sizeof(taskKeyPool)/sizeof(taskKeyPool[0]),
   .param     = NULL,
   .func      = taskKey,
   .hook      = defaultHook,
   .begin     = taskKeyBegin,
   .end       = rien,
};

void* taskKey(void* a)
{
   keys_t keys = {               //esta estructura la cargo en el stadk de la propia tarea
      .state       = {1,1},      //lleva el estado actual de las teclas
      .tec1AndTec2 = 1,          //estado de la combinatoria de Tec1 y Tec2
      .name        = {TEC1,TEC2} //nombres de sapi para manejar las entradas
   };
   uint32_t pressedKey;

   Set_Irq_Tec(TEC1_INDEX ,0 ,4 ,BOTH_EDGE);     // TEC1
   Set_Irq_Tec(TEC2_INDEX ,0 ,8 ,BOTH_EDGE);     // TEC2
   while(1) {
      eventTake              ( &keyEvent,(void* )&pressedKey); // espero que se oprima una tecla y recibo ademas cuel es en el evento
      updateKeyState         ( &keys,pressedKey );             // actualizo su estado
      updateTec1AndTec2State ( &keys            );             // actualizo ahora el estao conjunto de tec1 y tec2
   }
}

void updateKeyState(keys_t* keys,uint8_t i)
{
   static uint8_t newState;
   newState=gpioRead(keys->name[i]);                          // leo el estado actuala
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

void Set_Irq_Tec(uint8_t Irq_Ch,uint8_t Port, uint8_t Pin, Edges_T Edge )
{
    Chip_SCU_GPIOIntPinSel     ( Irq_Ch               ,Port ,Pin        ) ;
    Chip_PININT_ClearIntStatus ( LPC_GPIO_PIN_INT     ,PININTCH( Irq_Ch ));
    Chip_PININT_SetPinModeEdge ( LPC_GPIO_PIN_INT     ,PININTCH( Irq_Ch ));
    NVIC_SetPriority           ( PIN_INT0_IRQn+Irq_Ch ,255              ) ;
    NVIC_ClearPendingIRQ       ( PIN_INT0_IRQn+Irq_Ch                   ) ;
    NVIC_EnableIRQ             ( PIN_INT0_IRQn+Irq_Ch                   ) ;
    switch (Edge) {
       case RISING_EDGE:
             Chip_PININT_EnableIntHigh  ( LPC_GPIO_PIN_INT, PININTCH( Irq_Ch ));
             break;
       case FALLING_EDGE:
             Chip_PININT_EnableIntLow   ( LPC_GPIO_PIN_INT, PININTCH( Irq_Ch ));
             break;
       case BOTH_EDGE:
       default:
             Chip_PININT_EnableIntHigh  ( LPC_GPIO_PIN_INT, PININTCH( Irq_Ch ));
             Chip_PININT_EnableIntLow   ( LPC_GPIO_PIN_INT, PININTCH( Irq_Ch ));
          break;
    }
}
void GPIO1_IRQHandler(void)
{
   Chip_PININT_ClearIntStatus ( LPC_GPIO_PIN_INT ,PININTCH(TEC1_INDEX ));
   if(eventGiveTout4Isr ( &keyEvent ,(void*)0 ,1 ))
     triggerPendSv4Isr();
}
void GPIO2_IRQHandler(void)
{
   Chip_PININT_ClearIntStatus ( LPC_GPIO_PIN_INT ,PININTCH(TEC2_INDEX ));
   if(eventGiveTout4Isr ( &keyEvent ,(void*)1 ,1 ))
    triggerPendSv4Isr();
}
