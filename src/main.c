#include "main.h"         // <= Su propia cabecera
#include "string.h"
#include "sapi.h"        // <= Biblioteca sAPI
#include "switcher.h"

taskContext*   taskList[10];
uint32_t       taskCount   = 0;
uint32_t       runningTask = 0;
taskContext    *runningContext;
taskContext    t1,t2,t3;
taskContext    kernelContext;

bool stackPush(uint32_t data, taskContext* c)/*{{{*/
{
   c->pool_index--;
   *c->pool_index=data;
}/*}}}*/
bool taskCreate(const char* name, void* (*func)(void*),void* param, taskContext* c, void* (*hook)(void*))/*{{{*/
{
   strcpy(c->name,name);                         // la tarea guard su nombre
   c->pool_index=&(c->pool[MIN_STACK]);          // el pool es estativo y fijo, se guarda el puntero a la ultima posicion porque funcionara como stak de arriba para abao
   stackPush ( 1<<24                       ,c ); // PSR     //comiezo a pushear todo, bit de modo thumb en uno siempre
   stackPush ( (uint32_t)func              ,c ); // PC
   stackPush ( (uint32_t)hook              ,c ); // LR
   stackPush ( 0                           ,c ); // r12

   stackPush ( 0                           ,c ); // r3
   stackPush ( 0                           ,c ); // r2
   stackPush ( 0                           ,c ); // r1
   stackPush ( (uint32_t)param             ,c ); // r0

   stackPush ( 0                           ,c ); // r4
   stackPush ( 0                           ,c ); // r5
   stackPush ( 0                           ,c ); // r6
   stackPush ( 0                           ,c ); // r7

   stackPush ( 0                           ,c ); // r8
   stackPush ( 0                           ,c ); // r9
   stackPush ( 0                           ,c ); // r10
   stackPush ( 0                           ,c ); // r11
   taskList[taskCount++]=c;                      // habemos una tarea extra
}/*}}}*/
bool kernelCreate(const char* name, void* (*func)(void*),void* param, taskContext* c, void* (*hook)(void*))/*{{{*/
{
   strcpy(c->name,name);                         // la tarea guard su nombre
   c->pool_index=&(c->pool[MIN_STACK]);          // el pool es estativo y fijo, se guarda el puntero a la ultima posicion porque funcionara como stak de arriba para abao
   stackPush ( 1<<24                       ,c ); // PSR     //comiezo a pushear todo, bit de modo thumb en uno siempre
   stackPush ( (uint32_t)func              ,c ); // PC
   stackPush ( (uint32_t)hook              ,c ); // LR
   stackPush ( 0                           ,c ); // r12

   stackPush ( 0                           ,c ); // r3
   stackPush ( 0                           ,c ); // r2
   stackPush ( 0                           ,c ); // r1
   stackPush ( (uint32_t)param             ,c ); // r0

   stackPush ( 0                           ,c ); // r4
   stackPush ( 0                           ,c ); // r5
   stackPush ( 0                           ,c ); // r6
   stackPush ( 0                           ,c ); // r7

   stackPush ( 0                           ,c ); // r8
   stackPush ( 0                           ,c ); // r9
   stackPush ( 0                           ,c ); // r10
   stackPush ( 0                           ,c ); // r11
}/*}}}*/
bool taskDelete(taskContext* c)/*{{{*/
{
  taskCount--;    //tarea menos
}/*}}}*/
void* task1(void* a)/*{{{*/
{
   uint32_t i;
   while(1) {
      for (i=0;i<5000000;i++)
         ;
      gpioToggle(LED1);
   }
   return NULL;
}/*}}}*/
void* hook1(void* p) {}

void* task2(void* a)/*{{{*/
{
   uint32_t i;
   while(1) {
      for (i=0;i<4000000;i++)
         ;
      gpioToggle(LED2);
   }
   return NULL;
}/*}}}*/
void* hook2(void* p) {}

void* task3(void* a)/*{{{*/
{
   uint32_t i,j;
   for(j=0;j<10;j++) {
      for (i=0;i<2000000;i++)
         ;
       gpioToggle(LED3);
   }
   return NULL;
}/*}}}*/
void* hook3(void* p)/*{{{*/
{
   uint32_t i;
   while(1) {
      for (i=0;i<1000000;i++)
         ;
      gpioToggle(LEDR);
   }
   return NULL;
}/*}}}*/

int main( void )
{
   boardConfig();
   taskCreate   ( "tarea1" ,task1      ,(void* )0x01 ,&t1            ,hook1)     ;
   taskCreate   ( "tarea2" ,task2      ,(void* )0x02 ,&t2            ,hook2)     ;
   taskCreate   ( "tarea3" ,task3      ,(void* )0x03 ,&t3            ,hook3)     ;
   kernelCreate ( "kernel" ,kernelTask ,(void* )0x04 ,&kernelContext ,hookKernel);

   runningTask=0;
   runningContext=&kernelContext;
   SysTick_Config( (SystemCoreClock * (tick_t)50) / 1 );

   __WFI();

   while( TRUE )
      ;
   return 0;
}

void* kernelTask(void* p) //round robin por ahora (pero con hook)
{
   if(++runningTask>=taskCount)
      runningTask=0;
   runningContext=taskList[runningTask];
}
void* hookKernel(void* p)/*{{{*/
{
   return NULL;
}/*}}}*/
