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

bool semphrInit  ( semphr_t* s );
bool semphrTake  ( semphr_t* s );
bool semphrGive  ( semphr_t* s );
/*==================[end of file]============================================*/
#endif
