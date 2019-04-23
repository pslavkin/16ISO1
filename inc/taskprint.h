#ifndef TASKPRINT
#define TASKPRINT

#define MAX_PRINT_MSG  10
#define MAX_MSG_LENGTH 250

extern queue_t    printQueue;
extern taskParams_t taskPrintParams;

void* taskPrint      ( void* a              );
char* ftostr         ( float fVal,char* str );

#endif
