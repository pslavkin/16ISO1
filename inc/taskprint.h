#ifndef TASKPRINT
#define TASKPRINT

#include "queue.h"

#define MAX_PRINT_MSG   20
#define MAX_MSG_LENGTH 250

extern queue_t       printQueue;
extern mutex_t       printMutex;
extern taskParams_t  taskPrintParams;

void* taskPrint ( void* a                 );
int printUART   ( const char *format, ... );

#endif
