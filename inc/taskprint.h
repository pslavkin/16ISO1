#ifndef TASKPRINT
#define TASKPRINT

#define MAX_PRINT_MSG  10
#define MAX_MSG_LENGTH 200

extern queue_t    printQueue;
extern taskParams_t taskPrintParams;

void  initPrintQueue ( void                 );
void* taskPrint      ( void* a              );
char* ftostr         ( float fVal,char* str );

#endif
