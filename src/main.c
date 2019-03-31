#include "sapi.h"
#include "os.h"
#include "task1.h"
#include "task2.h"
#include "task3.h"
#include "taskidle.h"

DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE

int main( void )
{
   boardConfig();
   debugPrintConfigUart( UART_USB, 115200 );

   NVIC_EnableIRQ(PendSV_IRQn);
   NVIC_SetPriority(PendSV_IRQn,(1<<__NVIC_PRIO_BITS) -1);

   initTasks();
   taskCreate ( &taskKernelParams ,9 );
   taskCreate ( &taskIdleParams   ,1 );
   taskCreate ( &task1Params      ,1 );
   taskCreate ( &task2Params      ,1 );
   taskCreate ( &task3Params      ,1 );
   Init_SysTick();


   while( TRUE )
      ;
   return 0;
}

