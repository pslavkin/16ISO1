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

   kernelCreate ( "kernel"    ,kernelTask ,(void* )0x04 ,&kernelContext ,0,hookKernel);
   taskCreate   ( "tareaIdle" ,taskIdle   ,(void* )0x01 ,&contextIdle   ,1,hookIdle)  ;
   taskCreate   ( "tarea1"    ,task1      ,(void* )0x01 ,&context1      ,1,hook1)     ;
   taskCreate   ( "tarea2"    ,task2      ,(void* )0x02 ,&context2      ,1,hook2)     ;
   taskCreate   ( "tarea3"    ,task3      ,(void* )0x03 ,&context3      ,1,hook3)     ;

   SysTick_Config( (SystemCoreClock * (tick_t)50) / 100 );
   __WFI();

   while( TRUE )
      ;
   return 0;
}

