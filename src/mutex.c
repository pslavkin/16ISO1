#include <stdbool.h>
#include "os.h"
#include "semphr.h"
#include "mutex.h"

bool mutexInit ( mutex_t* m )
{
   m->count = 1; // arranca en 1, que seria desbloqueado en 1 unidad
   m->max   = 1; //
};
bool mutexLock ( mutex_t* m )
{
   return semphrTake(m);
}
bool mutexUnlock ( mutex_t* m )
{
   return semphrGive(m,1);
};
//-------------------------------------------
