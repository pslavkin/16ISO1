#ifndef QUEUE
#define QUEUE

#include <stdint.h>
#include <stdbool.h>

typedef struct    queue_struct {   //ok, si por ahora esta estructura no es muy util.. pero tengo otros planes...
   bool locked;
} queue_t;

bool queueInit   ( queue_t* m );
bool queueLock   ( queue_t* m );
bool queueUnlock ( queue_t* m );
/*==================[end of file]============================================*/
#endif
