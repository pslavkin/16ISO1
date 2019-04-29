#ifndef ISR
#define ISR

void addIsr ( void (*f )(void), uint8_t irqCh, uint8_t prior);
void delIsr       ( uint8_t irqch );
void osIsrHandler ( uint8_t irqCh );
#endif
