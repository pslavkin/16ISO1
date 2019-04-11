#ifndef TASK1
#define TASK1

#include "semphr.h"
/*==================[tipos de datos declarados por el usuario]===============*/
extern semphr_t   printfMutex;
extern semphr_t   printfSemphr;
extern taskParams task1Params;
/*==================[declaraciones de datos externos]========================*/
void  initPrintfMutex  ( void    );
void  initPrintfSemphr ( void    );
void* task1            ( void* a );
void* hook1            ( void* p );
/*==================[end of file]============================================*/
#endif
