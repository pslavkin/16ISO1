#ifndef SEMPHR
#define SEMPHR

#include <stdint.h>
#include <stdbool.h>

typedef struct    semphr_struct {   //ok, si por ahora esta estructura no es muy util.. pero tengo otros planes...
   bool locked;
} semphr_t;

bool mutexInit   ( semphr_t* m );
bool mutexLock   ( semphr_t* m );
bool mutexUnlock ( semphr_t* m );
/*==================[end of file]============================================*/
#endif
