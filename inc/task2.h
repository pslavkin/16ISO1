#ifndef TASK2
#define TASK2

extern taskParams_t task2Params;
extern semphr_t task2Semphr;
extern event_t  task2Event;

void* task2      ( void* a );

#endif
