#include "app.h"         // <= Su propia cabecera
#include "sapi.h"        // <= Biblioteca sAPI

   volatile int b=1;
   volatile int c=1;

void task3(void* a)
{
         b++;
}

uint32_t i,j;
volatile int first_shot=1;

void task1(void* a)
{
   while(1) { 
      for (i=0;i<10000000;i++)
         ;
      gpioToggle(LED1);
   }
}
void task2(void* a)
{
   while(1) {
      for (j=0;j<10009000;j++)
         ;
         gpioToggle(LED2);
   }
}

//void SysTick_Handler(void)
//{
//   c++;
//}

uint32_t task1_stask[100];
uint32_t task2_stask[100];

uint32_t task1_p,task2_p;
uint32_t Actual_Task=0;






int main( void )
{
   boardConfig();
   task1_p=(task1_stask+100-8);
   task1_stask[100-1]=1<<24;
   task1_stask[100-2]=task1;
   task1_stask[100-3]=task1_stask;
   task1_stask[100-9]=0xFFFFFFF9;

   task2_p=(task2_stask+100-8);
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
