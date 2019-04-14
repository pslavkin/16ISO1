#ifndef TASKPRINT
#define TASKPRINT

#define MAX_PRINT_MSG  10
#define MAX_MSG_LENGTH 80

extern queue_t    printQueue;
extern taskParams taskPrintParams;

void  initPrintQueue ( void    );
void* taskPrint      ( void* a );
void* hookPrint      ( void* p );

#endif
