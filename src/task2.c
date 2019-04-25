#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "semphr.h"
#include "event.h"
#include "mutex.h"
#include "systick.h"
#include "task1.h"
#include "task2.h"
#include "taskprint.h"
#include "stat.h"
#include "convert.h"

uint32_t task2Pool[SMALL_STACK];
semphr_t ledSemphr;

void initTask2(void)
{
   semphrInit(&ledSemphr,1);  //semaforo binario, ya que lo uso para ping pong entre task1 y esta
}

taskParams_t task2Params = {
   .name      = "task2 Leds",
   .pool      = task2Pool,
   .poolSize  = sizeof(task2Pool)/sizeof(task2Pool[0]),
   .param     = NULL,
   .func      = task2,
   .hook      = defaultHook,
   .init      = initTask2,
};

void setLedOff(void)
{
   gpioWrite(LEDG,0);
   gpioWrite(LEDR,0);
   gpioWrite(LED1,0);
   gpioWrite(LEDB,0);
}
void setLed(uint8_t color)
{
   //se podria hacer uso de la secuencialidad del enum de LEDG, LEDR y demas pero se decido
   //hacerlo mas generico sin importar si estan o no en secuencia usando un switch
   setLedOff();      //primeo apago y luego enciendo el que corresponda. Se podria evitar el micro flickeo apagando solo los que no  se van a enceder, perp para esta aplicacion es indistinto
   switch(color) {
      case 0:
         gpioWrite(LEDG,1);
         break;
      case 1:
         gpioWrite(LEDR,1);
         break;
      case 2:
         gpioWrite(LED1,1);
         break;
      case 3:
         gpioWrite(LEDB,1);
         break;
      default:
         break;
   }
}

void* task2(void* a)
{
   timing_t* t;
   while(1) {
      //notar que me llega el puntero a la estructgura, no la copia, hay que tener en cuenta
      //los tiempo asociados para que la estructura de origen no se escriba antes de que esta
      //tarea termine de leer, es literalmente imposible en este caso, pero se asegura dandole
      //mayor propridad a esta tarea cosa de que al hacer el evengive inmediatamente esta tarea
      //tome el control y lea sin que pueda haber condicion de carrera
      eventTake  ( &ledEvent  ,(void* )&t );
      setLed     ( t->color               ); // tom el color y seteo el led correspondiente
      taskDelay  ( t->sumT1T2             ); // espero el tiempo correspondiente que esta en el campo sumT1T2
      setLedOff  (                        ); // apago y vuelvo a empeazr
      semphrGive ( &ledSemphr ,1          ); // con este semafor habilito a la tarea de reportes y calculos que continue con el proximo procesamiento si lo hubiera y esta en cola
   }
   return NULL;
}
