#ifndef SEMPHR
#define SEMPHR

#include <stdint.h>
#include <stdbool.h>

typedef struct    semphr_struct {   //ok, si por ahora esta estructura no es muy util.. pero tengo otros planes...
   uint32_t count;      //lleva cuenta de cuantos gives se hicieron
   uint32_t max;        //maximo de gives admitidos
} semphr_t;

bool mutexInit   ( semphr_t* m );
bool mutexLock   ( semphr_t* s );
bool mutexUnlock ( semphr_t* s );

bool semphrInit   ( semphr_t* s, uint32_t count );
bool semphrTake   ( semphr_t* m );
bool semphrGive   ( semphr_t* m , uint32_t qty);
/*==================[end of file]============================================*/
#endif
