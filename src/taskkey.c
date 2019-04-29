#include <stdint.h>
#include "os.h"
#include "sapi.h"
#include "queue.h"
#include "systick.h"
#include "taskkey.h"
#include "task1.h"
#include "isr.h"
#include "taskprint.h"

//este evento es usado  por los ahndler de la tecla para no solo avisar a la tarea  que se
//apreto una tecla sino que ademas el evento le dice cual, con un evento que recibe el
//parametro y que puede tener mas de 1 unidad, puedo recibir las dos teclas y no se puerden los
//datos  TODO: analizar que pasa si vienen los dos botones al mismo tiempo.. mepa que no anda,
//porque se pisa el dato.. habria que poner 2 eventos..nunca me paso, pero podria..
event_t keyEvent; 

void updateKeyState         ( keys_t* keys,uint8_t i                                 );
void updateTec1AndTec2State ( keys_t* keys                                           );
void Set_Irq_Tec            ( uint8_t Irq_Ch,uint8_t Port, uint8_t Pin, Edges_T Edge );
void printKeyStruct         ( keys_t* keys                                           );
void gpio1Handler           ( void                                                   );
void gpio2Handler           ( void                                                   );
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

   Set_Irq_Tec ( TEC1_INDEX   ,0                        ,4 ,BOTH_EDGE ); //habilito irq del gpio tec1
   Set_Irq_Tec ( TEC2_INDEX   ,0                        ,8 ,BOTH_EDGE ); //habilito irq del canal tec2 
   addIsr      ( gpio1Handler ,PIN_INT0_IRQn+TEC1_INDEX ,255          ); //instalo el handler correposndiente a la tec 
   addIsr      ( gpio2Handler ,PIN_INT0_IRQn+TEC2_INDEX ,255          ); //instalo el handler correposndiente a la tec 

   while(1) {
      eventTake              ( &keyEvent,(void* )&pressedKey); // espero que se oprima una tecla y recibo ademas cuel es en el evento
      updateKeyState         ( &keys,pressedKey );             // actualizo su estado
      updateTec1AndTec2State ( &keys            );             // actualizo ahora el estao conjunto de tec1 y tec2
   }
}

//en vez de una maquina de estados como se hizo en rtos1, como para probar otro enfoque lo que
//hago es medir cada tecla y el xor de las dos juntas, esperando que pase por el estado 0,0 y
//luego espero el 1,1, minentras que cada vez que se mueven tomo el tiempo. Si vienen flancos
//en el medio, tambien tomo tiempos con lo cual me quedo con el ultimo valido y no muestro
//error ni nada, lo considero valido. No implemento debounce, por lo del capa en el boton y
//para no mezclar mas el codigo asi se interpreta la idea. 
//Esta funcion entonces actualiza el estado de la tecla apretada
void updateKeyState(keys_t* keys,uint8_t i)
{
   static uint8_t newState;
   newState=gpioRead(keys->name[i]);                          // leo el estado actuala
   if(newState != keys->state[i]) {                           // si cambio
      keys->state[i]=newState;                                // actualizo
      (newState?keys->riseTime:keys->fallTime)[i]=getTicks(); // cargo el tiempo para rise o fall
   }
}
//mientras que esta funcion actualiza el estado de las 2 en conjunto reconociendo cuando pasa
//de 0,0 a 1,1 y es alli donde ocurre el envio de un evento a la cola para que se procese.
//Notar que al enviar por copia, mientras se procesa un mensaje, la tarea de botones puede
//seguir trabajando yu modificar la estructura local de tiempo y estados ya que el anterior se
//envio por copia. Dependiendo de la cantidad de lugar en la cola que se disponga es la
//cantidad de eventos consecutivos antes de bloquear.
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

//esta funcion habilita el rising/falling o ambos para la tecla elegida
void Set_Irq_Tec(uint8_t Irq_Ch,uint8_t Port, uint8_t Pin, Edges_T Edge )
{
    Chip_SCU_GPIOIntPinSel     ( Irq_Ch               ,Port ,Pin        ) ;
    Chip_PININT_ClearIntStatus ( LPC_GPIO_PIN_INT     ,PININTCH( Irq_Ch ));
    Chip_PININT_SetPinModeEdge ( LPC_GPIO_PIN_INT     ,PININTCH( Irq_Ch ));
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

//estas son las funciones que se instalan en el osHandlerVector para ser llamadas ante una
//interrupcion de teclas.
void gpio1Handler(void)
{
   Chip_PININT_ClearIntStatus ( LPC_GPIO_PIN_INT ,PININTCH(TEC1_INDEX )); // limpia el flag
   if(eventGive4Isr ( &keyEvent ,(void*)0 ,1 ))                           // una api diferente para enviar dentro de una irq un evento.. leer como funciona en event.h
     triggerPendSv4Isr();                                                  //pero si desperto algun take, llama a pend para que recorra a ver si le toca0
}
//idem gpio1handler
void gpio2Handler(void)
{
   Chip_PININT_ClearIntStatus ( LPC_GPIO_PIN_INT ,PININTCH(TEC2_INDEX ));
   if(eventGive4Isr ( &keyEvent ,(void*)1 ,1 ))
    triggerPendSv4Isr();
}
