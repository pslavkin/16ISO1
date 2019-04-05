#include <stdint.h>
#include "pendsv.h"
#include "sapi.h"

void initPendsv(void)
{
   //la prioridad TIENE que ser la mas baja para no comprometer situaciones de conflicto entre
   //irq's simultanteas
   NVIC_SetPriority(PendSV_IRQn,(1<<__NVIC_PRIO_BITS) -1);
}

