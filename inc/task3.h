#ifndef TASK3
#define TASK3

typedef struct shared_struct {
   float    f;
   uint32_t u;
   bool     b;
   uint8_t  s[10];
} shared_t;

extern taskParams_t task3Params;
void* task3      ( void* a );
extern event_t task3Event;


#endif
