#ifndef SYSTICK
#define SYSTICK
/*==================[declaraciones de datos externos]========================*/
void initSystick       ( void );
void SysTick_Handler   ( void );
void enableSystickIrq  ( void );
void disableSystickIrq ( void );
/*==================[end of file]============================================*/
#endif