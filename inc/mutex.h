#ifndef MUTEX
#define MUTEX

#include <stdbool.h>

#define mutex_t  event_t

bool mutexInit   ( mutex_t* m );
bool mutexLock   ( mutex_t* m );
bool mutexUnlock ( mutex_t* m );

#endif
