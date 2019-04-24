#ifndef TASKPRINT
#define TASKPRINT

#define MAX_PRINT_MSG   2
#define MAX_MSG_LENGTH 250

extern queue_t       printQueue;
extern mutex_t       printMutex;
extern taskParams_t  taskPrintParams;

void* taskPrint      ( void* a              );

#endif
