#include <stdint.h>
#include "pendsv.h"
#include "sapi.h"

void initPendsv(void)
{
   NVIC_EnableIRQ(PendSV_IRQn);
   NVIC_SetPriority(PendSV_IRQn,(1<<__NVIC_PRIO_BITS) -1);
}
