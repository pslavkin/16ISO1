#ifndef TASK1
#define TASK1

#include "semphr.h"
#include "queue.h"

extern semphr_t     task1Semphr;
extern taskParams_t task1Params;

void* task1            ( void* a );
#endif
