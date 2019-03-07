#include "app.h"         // <= Su propia cabecera
#include "sapi.h"        // <= Biblioteca sAPI

   volatile int b=1;
   volatile int c=1;

void task3(void* a)
{
         b++;
}


void task1(void* a)
{
         gpioToggle(LEDB);
}
void task2(void* a)
{
         gpioToggle(LEDR);
}

void SysTick_Handler(void)
{
   c++;
}

uint32_t task1_stask[100];
uint32_t task2_stask[100];

uint32_t task1_p,task2_p;






int main( void )
{
   boardConfig();
   task1_stask[100-1]=1<<24;
   task1_stask[100-2]=task1;
   task1_stask[100-3]=task1_stask;
   task1_stask[100-9]=0xFFFFFFF9;

   task2_stask[100-1]=1<<24;
   task2_stask[100-2]=task2;
   task2_stask[100-3]=task2_stask;
   task2_stask[100-9]=0xFFFFFFF9;

   SysTick_Config( (SystemCoreClock * (tick_t)50) / 1000 );

   while( TRUE ) {
   //:w
   //task2(NULL);
    task3(NULL);
   }

   return 0;
}
