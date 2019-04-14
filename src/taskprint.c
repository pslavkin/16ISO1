#include <stdint.h>
#include <stdio.h>
#include "string.h"
#include "os.h"
#include "sapi.h"
#include "task1.h"
#include "mutex.h"
#include "taskprint.h"

uint32_t taskPrintPool[MIN_STACK];
uint8_t  cbPool       [(MAX_PRINT_MSG+1)*MAX_MSG_LENGTH];
circularBuffer_t cb;
queue_t printQueue;

taskParams taskPrintParams = {
   .name      = "taskPrint",
   .pool      = taskPrintPool,
   .pool_size = sizeof(taskPrintPool)/sizeof(taskPrintPool[0]),
   .param     = NULL,
   .func      = taskPrint,
   .hook      = defaultHook,
};
void initPrintQueue(void)
{
   circularBuffer_Init ( &cb,cbPool,MAX_PRINT_MSG,MAX_MSG_LENGTH );
   queueInit           ( &printQueue,&cb                         );
}
void* taskPrint(void* a)
{
   uint8_t data[MAX_MSG_LENGTH];
   while(1) {
      queueRead   ( &printQueue,data );
      gpioWrite   ( LEDG,true        );
 //     mutexLock   ( &printfMutex     ); // con este mutex me evito que si otra
         stdioPrintf ( UART_USB,data    );
//      mutexUnlock ( &printfMutex     ); // con este mutex me evito que si otra
      gpioWrite   ( LEDG,false       );
   }
   return NULL;
}
