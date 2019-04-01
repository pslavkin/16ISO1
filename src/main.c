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
   boardConfig();
   debugPrintConfigUart( UART_USB, 115200 );

   initPendsv ( );

   initTasks  ( );
   taskCreate ( &task1Params      ,2 );
   taskCreate ( &task2Params      ,2 );
   taskCreate ( &task3Params      ,2 );

   initSystick ( );
   yield       ( );

   while( TRUE )
      ;
   return 0;
}

