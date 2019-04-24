#include <stdint.h>
#include "os.h"
#include "taskkernel.h"
#include "systick.h"
#include "sapi.h"
#include "pendsv.h"
#include "semphr.h"

bool semphrInit ( semphr_t* s, uint32_t count )
{
  return  eventInit(s,count);
};
//----------------------------------------------------------------------
bool semphrTake   ( semphr_t* s)
{
   return eventTakeTout(s,NULL,0);
}
bool semphrTakeTout   ( semphr_t* s, uint32_t tout )
{
   return eventTakeTout(s,NULL,tout);
};
bool semphrGive   ( semphr_t* s , uint32_t qty)
{
   return eventGiveTout( s ,NULL, qty ,0 );
}
bool semphrGiveTout   ( semphr_t* s , uint32_t qty, uint32_t tout)
{
   return eventGiveTout( s ,NULL, qty ,tout );
};
// ----------------------------------------------------------------------
