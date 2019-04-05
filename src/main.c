#include "sapi.h"
#include "os.h"
#include "task1.h"
#include "task2.h"
#include "task3.h"
#include "taskidle.h"
#include "systick.h"
#include "pendsv.h"

DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE

int main( void )
{
   boardConfig          (                  );
   debugPrintConfigUart ( UART_USB, 115200 );

   initPendsv ( );                 // se usa para solicitar que salte irq para el cambio de contexto

   initPrintfMutex ( );            //inicializa un mutex para evitar que varias tareas corrompan la impresion por uart de otra0
   initTasks       ( );            // inicializa las estructuras de control de tareas, lanza el taskkernel y el taskidle
   taskCreate ( &task1Params ,2 ); // 3 tareas de ejemplo. reciben una estructura con las opciones pero
   taskCreate ( &task2Params ,4 ); // se deja fuera la prioridad para visualizarlas aca
   taskCreate ( &task3Params ,6 );

   initSystick   ( );              // irq de base de tiempo
   triggerPendSv ( );              // solicita irq para que todo avance.

   while( TRUE )                   // nunca llegara aca, porque en el primer cambio de
      ;                            // contexto se guarda el contexto del main como el contexto del kernelTask
   return 0;                       // de modo que el sp del kernel stack pasa a ser el de este main.
}

