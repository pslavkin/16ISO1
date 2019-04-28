#include "sapi.h"
#include "os.h"
#include "task1.h"
#include "task2.h"
#include "task3.h"
#include "taskkey.h"
#include "taskmenu.h"
#include "tasktemplate.h"
#include "taskprint.h"
#include "taskidle.h"
#include "systick.h"
#include "pendsv.h"

DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE

int main( void )
{
   boardConfig          (                  );
   debugPrintConfigUart ( UART_USB, 115200 );

   initPendsv ( );                        // se usa para solicitar que salte irq para el cambio de contexto
   initTasks  ( );                        // inicializa las estructuras de control de tareas, lanza el taskkernel y el taskidle

   taskCreate ( &taskPrintParams    ,3 ); // tarea que imprime mensajes que le llegan por cola
   taskCreate ( &taskMenuParams     ,2 ); // tarea que lee la UART y presenta una serie de opciones con '?' 
   taskCreate ( &task1Params        ,2 ); // tarea que calcula los delta temporales y los reparte a las tareas de leds y de impresion de reporte
   taskCreate ( &task2Params        ,2 ); // tarea que gestiona el delay de los leds que le llega por evento y devuelve un semaforo a la tarea de calculo cuando termina para que siga procesando el siguiente
   taskCreate ( &task3Params        ,1 ); // dummy, para mostrar la creacion y borrado on run-time desde el menu de opciones
   taskCreate ( &taskKeyParams      ,8 ); // polea las teclas y guarda los tiempos de fall y rise en una estructuaa
   taskCreate ( &taskTemplateParams ,5 ); // dummy para copiar y pegar y hacer otra tarea, ahora funciona como blinky

   initSystick   ( );                     // irq de base de tiempo
   triggerPendSv ( );                     // solicita irq para que todo avance.

   while( TRUE )                          // nunca llegara aca, porque en el primer cambio de
      ;                                   // contexto se guarda el contexto del main como el contexto del kernelTask
   return 0;                              // de modo que el sp del kernel stack pasa a ser el de este main.
}

