#include <stdint.h>
#include <stdio.h>
#include "string.h"
#include "os.h"
#include "systick.h"
#include "sapi.h"
#include "task1.h"
#include "mutex.h"
#include "taskprint.h"

uint32_t taskPrintPool[REASONABLE_STACK];
int8_t  cbPool       [(MAX_PRINT_MSG+1)*MAX_MSG_LENGTH];
circularBuffer_t cb;
queue_t printQueue;

void taskPrintInit(void)
{
   queueInit ( &printQueue,&cb,cbPool,MAX_PRINT_MSG,MAX_MSG_LENGTH );
}

taskParams_t taskPrintParams = {
   .name      = "taskPrint",
   .pool      = taskPrintPool,
   .poolSize  = sizeof(taskPrintPool)/sizeof(taskPrintPool[0]),
   .param     = NULL,
   .func      = taskPrint,
   .hook      = defaultHook,
   .init      = taskPrintInit,
};
void* taskPrint(void* a)
{
   uint8_t data[MAX_MSG_LENGTH];
   while(1) {
      while(queueReadTout ( &printQueue ,data, msec2Ticks(10000))==false)
         uartWriteString ( UART_USB    ,"nada para imprimir\r\n"  );
      gpioWrite       ( LEDG        ,true  );
      uartWriteString ( UART_USB    ,data  );
      gpioWrite       ( LEDG        ,false );
   }
   return NULL;
}
//----convierte float a string para validar float preemptive--------------
char* ftostr  (float fVal,char* str)
{
 int32_t Entera, Dec;
 uint8_t Len;
 uint8_t Sign[]="+";

 Entera  = fVal;
 Dec     = (int32_t)(fVal * 1000);
 Dec    %= 1000;
 if(fVal<0) {
    Dec     = -Dec;
    Entera  = -Entera;
    Sign[0] = '-';
 }
 Len=sprintf(str,"%s%d",Sign,Entera);
 str[Len++] = '.';
 sprintf(str+Len,"%d\r\n",Dec);
 return str;
}

