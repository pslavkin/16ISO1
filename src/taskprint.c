#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include "string.h"
#include "os.h"
#include "systick.h"
#include "sapi.h"
#include "task1.h"
#include "mutex.h"
#include "taskprint.h"

uint32_t taskPrintPool[BIG_STACK];
int8_t  printCbPool   [cbPoolSpace(MAX_PRINT_MSG,MAX_MSG_LENGTH)];
circularBuffer_t printCb;
queue_t printQueue;
mutex_t printMutex;

void taskPrintInit(void)
{
   queueInit ( &printQueue,&printCb,printCbPool,MAX_PRINT_MSG,MAX_MSG_LENGTH );
   mutexInit ( &printMutex);
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
//         uartWriteString ( UART_USB    ,"nada para imprimir\r\n"  );
         ;
      gpioWrite          ( LED2        ,true  );
      mutexLock          ( &printMutex        );
         uartWriteString ( UART_USB    ,data  );
      mutexUnlock        ( &printMutex        );
      gpioWrite          ( LED2        ,false );
   }
   return NULL;
}

int printUART(const char *format, ...)
{
  va_list params;
  uint8_t buf[MAX_MSG_LENGTH];
  va_start   ( params, format );
  vsnprintf  ( buf,sizeof(buf ),format,params);
  queueWrite ( &printQueue,buf );
}
