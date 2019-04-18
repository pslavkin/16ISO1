#ifndef TASK1
#define TASK1

#include "semphr.h"
#include "queue.h"
/*==================[tipos de datos declarados por el usuario]===============*/
extern semphr_t     printfMutex;
extern semphr_t     printfSemphr;
extern taskParams_t task1Params;
/*==================[declaraciones de datos externos]========================*/
void  initPrintfMutex  ( void    );
void  initPrintfSemphr ( void    );
void  initPrintfQueue  ( void    );
void* task1            ( void* a );
/*==================[end of file]============================================*/
#endif
