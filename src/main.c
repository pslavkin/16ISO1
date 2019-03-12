#include "main.h"         // <= Su propia cabecera
#include "string.h"
#include "sapi.h"        // <= Biblioteca sAPI

taskContext*   taskList[10];
uint32_t       taskCount   = 0;
uint32_t       runningTask = 0;
taskContext    t1,t2,t3,tn;

bool stackPush(uint32_t data, taskContext* c)/*{{{*/
{
   c->pool_index--;
   *c->pool_index=data;
}/*}}}*/
bool taskCreate(const char* name, void (*func)(void*),taskContext* c)/*{{{*/
{
   strcpy(c->name,name);                        //la tarea guard su nombre
   c->pool_index=&(c->pool[MIN_STACK]);         //el pool es estativo y fijo, se guarda el puntero a la ultima posicion porque funcionara como stak de arriba para abao
   stackPush ( 1<<24                       ,c ); // PSR     //comiezo a pushear todo0
   stackPush ( (uint32_t)func              ,c ); // LR
   stackPush ( (uint32_t)(c->pool_index-6) ,c ); // MSP
   stackPush ( 0                           ,c ); // r12

   stackPush ( 0                           ,c ); // r3
   stackPush ( 0                           ,c ); // r2
   stackPush ( 0                           ,c ); // r1
   stackPush ( 0                           ,c ); // r0

   stackPush ( 0                           ,c ); // r4
   stackPush ( 0                           ,c ); // r5
   stackPush ( 0                           ,c ); // r6
   stackPush ( 0                           ,c ); // r7
   stackPush ( 0                           ,c ); // r8
   stackPush ( 0                           ,c ); // r9
   stackPush ( 0                           ,c ); // r10
   stackPush ( 0                           ,c ); // r11
   taskList[taskCount++]=c;                     //habemos una tarea extra
}/*}}}*/
bool taskDelete(taskContext* c)/*{{{*/
{
  taskCount--;    //tarea menos
}/*}}}*/
void task1(void* a)/*{{{*/
{
   uint32_t i;
   while(1) {
      for (i=0;i<5000000;i++)
         ;
      gpioToggle(LED1);
   }
}/*}}}*/
void task2(void* a)/*{{{*/
{
   uint32_t i;
   while(1) {
      for (i=0;i<1000000;i++) {
         if(taskCount<3 && gpioRead( TEC1)==false)
            taskCreate("tarea3"   ,task3    ,&t3);
      }
      gpioToggle(LED2);

   }
}/*}}}*/
void task3(void* a)/*{{{*/
{
   uint32_t i;
   while(1) {
      for (i=0;i<5000000;i++)
         ;
         gpioToggle(LED3);
   }
}/*}}}*/
void taskNull(void* a)/*{{{*/
{
   while(1) {
   }
}/*}}}*/

int main( void )
{
   boardConfig();
   taskCreate("tarea1"   ,task1    ,&t1);
   taskCreate("tarea2"   ,task2    ,&t2);
   taskCreate("taskNull" ,taskNull ,&tn);

   runningTask=2; //arranca desde la ultima tarea
   taskDelete(&tn);  //pero justo despues la elimina, con lo cual la tarea 2 nunca se va a ejecutar, 
                     //y es justo lo que necesito. Porque entonces genere el stack inicial en donde se van a pushear todas las cosas en el 
                     //primer salto de la irq.. es una bosta el metodo pero funca por ahora
   SysTick_Config( (SystemCoreClock * (tick_t)50) / 1000 );

   while( TRUE ) {
   }

   return 0;
}
