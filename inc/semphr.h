#ifndef SEMPHR
#define SEMPHR

#include <stdint.h>
#include <stdbool.h>

typedef struct    semphr_struct {   //ok, si por ahora esta estructura no es muy util.. pero tengo otros planes...
   uint32_t count;
   uint32_t max;
} semphr_t;

bool mutexInit   ( semphr_t* m );
bool mutexLock   ( semphr_t* s );
bool mutexUnlock ( semphr_t* s );

bool semphrInit   ( semphr_t* s, uint32_t count );
bool semphrTake   ( semphr_t* m );
bool semphrGive   ( semphr_t* m , uint32_t qty);
/*==================[end of file]============================================*/
#endif
