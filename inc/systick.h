#ifndef SYSTICK
#define SYSTICK
/*===========================================================================*/
#define systickTimeDivisor  1000                         // no menos de 15, sino desborda el contador
                                                         // ojo con esta funcion que no se pase de rango.. fijate qye myltipkica x 1000
#define ticks2msec(t)      ((t*1000)/systickTimeDivisor) // paso de msegs a ticks
#define ticks2sec(t)       (t/systickTimeDivisor)        // paso de msegs a ticks

#define msec2Ticks(t)      ((t*systickTimeDivisor)/1000) // paso de msegs a ticks
#define sec2Ticks(t)       ((t*systickTimeDivisor)/1)    // paso de segs a ticks
#define min2Ticks(t)       ((t*systickTimeDivisor)*60)   // paso de mins a ticks
/*===========================================================================*/
void     initSystick       ( void                            );
void     SysTick_Handler   ( void                            );
void     enableSystickIrq  ( void                            );
void     disableSystickIrq ( void                            );
uint32_t getTicks          ( void                            );
uint32_t deltaTick         ( uint32_t first                  );
uint32_t diffTicks         ( uint32_t first, uint32_t second );
void     printUpTime       ( void                            );
/*==================[end of file]============================================*/
#endif
