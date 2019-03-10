#include "main.h"         // <= Su propia cabecera
#include "string.h"
#include "sapi.h"        // <= Biblioteca sAPI

uint32_t i,j,k;
volatile int first_shot=1;
taskContext* taskList[10];
uint32_t taskCount=0;
uint32_t runningTask=0;
taskContext t1,t2,t3,tn;

bool stackPush(uint32_t data, taskContext* c)
{
   c->pool_index--;
   *c->pool_index=data;
}
bool taskCreate(const char* name, void (*func)(void*),taskContext* c)
{
   strcpy(c->name,name);
   c->pool_index=&(c->pool[MIN_STACK]);
   // xx xx xx xx xx xx xx xx 
   stackPush ( 1<<24                       ,c ); // 100
   stackPush ( (uint32_t)func              ,c ); // 99
   stackPush ( (uint32_t)(c->pool_index-6) ,c ); // 98
   stackPush ( 0                           ,c ); // 98
   stackPush ( 0                           ,c ); // 98
   stackPush ( 0                           ,c ); // 98
   stackPush ( 0                           ,c ); // 98
   stackPush ( 0                           ,c ); // 98
   taskList[taskCount++]=c;
}
bool taskDelete(taskContext* c)
{
  taskCount--; 
}
void task1(void* a)
{
   while(1) {
      for (i=0;i<5000000;i++)
         ;
      gpioToggle(LED1);
   }
}
void task2(void* a)
{
   while(1) {
      for (j=0;j<1090000;j++) {
         if(taskCount<3 && gpioRead( TEC1)==false)
            taskCreate("tarea3"   ,task3    ,&t3);
      }
      gpioToggle(LED2);

   }
}
void task3(void* a)
{
   while(1) {
      for (k=0;k<1000000;k++)
         ;
         gpioToggle(LED3);
   }
}
void taskNull(void* a)
{
   while(1) {
   }
}





int main( void )
{
   boardConfig();
   taskCreate("tarea1"   ,task1    ,&t1);
   taskCreate("tarea2"   ,task2    ,&t2);
   taskCreate("taskNull" ,taskNull ,&tn);
   runningTask=2;
   taskDelete(&tn);
   SysTick_Config( (SystemCoreClock * (tick_t)50) / 1000 );

   while( TRUE ) {
   }

   return 0;
}
