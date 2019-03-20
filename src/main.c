#include "sapi.h"
#include "os.h"
#include "task1.h"
#include "task2.h"
#include "task3.h"

DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE

int main( void )
{
   boardConfig();
   debugPrintConfigUart( UART_USB, 115200 );

   taskCreate   ( "tarea1" ,task1      ,(void* )0x01 ,&context1      ,hook1);
   taskCreate   ( "tarea2" ,task2      ,(void* )0x02 ,&context2      ,hook2)     ;
   taskCreate   ( "tarea3" ,task3      ,(void* )0x03 ,&context3      ,hook3)     ;
   kernelCreate ( "kernel" ,kernelTask ,(void* )0x04 ,&kernelContext ,hookKernel);

   SysTick_Config( (SystemCoreClock * (tick_t)50) / 100 );
   __WFI();

   while( TRUE )
      ;
   return 0;
}

