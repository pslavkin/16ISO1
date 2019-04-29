#include <stdint.h>
#include "os.h"
#include "sapi.h"
#include "isr.h"

#define MAX_IRQ 52
void (*osHandlerVector[MAX_IRQ]) (void);

void addIsr(void (*f)(void), uint8_t irqCh, uint8_t prior)
{
   osHandlerVector[irqCh]=f;              //cargo en handler en el vector
   NVIC_SetPriority     ( irqCh ,prior ); //prioridad segun parametro
   NVIC_ClearPendingIRQ ( irqCh        ); //borro algun pendiente
   NVIC_EnableIRQ       ( irqCh        ); //habilito canal
}
void delIsr(uint8_t irqCh)
{
   osHandlerVector[irqCh]=rien;        //me gusta mandar a rien en vez de null porque null es garantia de reviente y rien no..
   NVIC_DisableIRQ ( irqCh );          //apago el canal
}
void osIsrHandler(uint8_t irqCh)
{
//   NVIC_ClearPendingIRQ   ( irqCh );    //puede que no haga falta..cereo que depende del periferico...
   osHandlerVector[irqCh] ( );            //llama a la funcion registrada en el vector
}
//apunto todos los veceores que estan definidos weak y los derivo a una unica funcion que por
//ahora solo llama a la funcion que tiene asociada en el vectr, pero en teoria alli es donde el
//os puede meter mano y decidir que y como hacer y tomar control.
void DAC_IRQHandler          ( void ) { osIsrHandler( DAC_IRQn        );}
void M0APP_IRQHandler        ( void ) { osIsrHandler( M0APP_IRQn      );}
void DMA_IRQHandler          ( void ) { osIsrHandler( DMA_IRQn        );}
void FLASH_EEPROM_IRQHandler ( void ) { osIsrHandler( RESERVED1_IRQn  );}
void ETH_IRQHandler          ( void ) { osIsrHandler( RESERVED2_IRQn  );}
void SDIO_IRQHandler         ( void ) { osIsrHandler( ETHERNET_IRQn   );}
void LCD_IRQHandler          ( void ) { osIsrHandler( SDIO_IRQn       );}
void USB0_IRQHandler         ( void ) { osIsrHandler( LCD_IRQn        );}
void USB1_IRQHandler         ( void ) { osIsrHandler( USB0_IRQn       );}
void SCT_IRQHandler          ( void ) { osIsrHandler( USB1_IRQn       );}
void RIT_IRQHandler          ( void ) { osIsrHandler( SCT_IRQn        );}
void TIMER0_IRQHandler       ( void ) { osIsrHandler( RITIMER_IRQn    );}
void TIMER1_IRQHandler       ( void ) { osIsrHandler( TIMER0_IRQn     );}
void TIMER2_IRQHandler       ( void ) { osIsrHandler( TIMER1_IRQn     );}
void TIMER3_IRQHandler       ( void ) { osIsrHandler( TIMER2_IRQn     );}
void MCPWM_IRQHandler        ( void ) { osIsrHandler( TIMER3_IRQn     );}
void ADC0_IRQHandler         ( void ) { osIsrHandler( MCPWM_IRQn      );}
void I2C0_IRQHandler         ( void ) { osIsrHandler( ADC0_IRQn       );}
void SPI_IRQHandler          ( void ) { osIsrHandler( I2C0_IRQn       );}
void I2C1_IRQHandler         ( void ) { osIsrHandler( I2C1_IRQn       );}
void ADC1_IRQHandler         ( void ) { osIsrHandler( SPI_INT_IRQn    );}
void SSP0_IRQHandler         ( void ) { osIsrHandler( ADC1_IRQn       );}
void SSP1_IRQHandler         ( void ) { osIsrHandler( SSP0_IRQn       );}
void UART0_IRQHandler        ( void ) { osIsrHandler( SSP1_IRQn       );}
void UART1_IRQHandler        ( void ) { osIsrHandler( USART0_IRQn     );}
void UART2_IRQHandler        ( void ) { osIsrHandler( UART1_IRQn      );}
void UART3_IRQHandler        ( void ) { osIsrHandler( USART2_IRQn     );}
void I2S0_IRQHandler         ( void ) { osIsrHandler( USART3_IRQn     );}
void I2S1_IRQHandler         ( void ) { osIsrHandler( I2S0_IRQn       );}
void SPIFI_IRQHandler        ( void ) { osIsrHandler( I2S1_IRQn       );}
void SGPIO_IRQHandler        ( void ) { osIsrHandler( SGPIO_INT_IRQn  );}
void GPIO0_IRQHandler        ( void ) { osIsrHandler( PIN_INT0_IRQn   );}
void GPIO1_IRQHandler        ( void ) { osIsrHandler( PIN_INT1_IRQn   );}
void GPIO2_IRQHandler        ( void ) { osIsrHandler( PIN_INT2_IRQn   );}
void GPIO3_IRQHandler        ( void ) { osIsrHandler( PIN_INT3_IRQn   );}
void GPIO4_IRQHandler        ( void ) { osIsrHandler( PIN_INT4_IRQn   );}
void GPIO5_IRQHandler        ( void ) { osIsrHandler( PIN_INT5_IRQn   );}
void GPIO6_IRQHandler        ( void ) { osIsrHandler( PIN_INT6_IRQn   );}
void GPIO7_IRQHandler        ( void ) { osIsrHandler( PIN_INT7_IRQn   );}
void GINT0_IRQHandler        ( void ) { osIsrHandler( GINT0_IRQn      );}
void GINT1_IRQHandler        ( void ) { osIsrHandler( GINT1_IRQn      );}
void EVRT_IRQHandler         ( void ) { osIsrHandler( EVENTROUTER_IRQn);}
void CAN1_IRQHandler         ( void ) { osIsrHandler( C_CAN1_IRQn     );}
void ADCHS_IRQHandler        ( void ) { osIsrHandler( ADCHS_IRQn      );}
void ATIMER_IRQHandler       ( void ) { osIsrHandler( ATIMER_IRQn     );}
void RTC_IRQHandler          ( void ) { osIsrHandler( RTC_IRQn        );}
void WDT_IRQHandler          ( void ) { osIsrHandler( WWDT_IRQn       );}
void M0SUB_IRQHandler        ( void ) { osIsrHandler( M0SUB_IRQn      );}
void CAN0_IRQHandler         ( void ) { osIsrHandler( C_CAN0_IRQn     );}
void QEI_IRQHandler          ( void ) { osIsrHandler( QEI_IRQn        );}

