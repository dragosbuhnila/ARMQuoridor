#include "button.h"
#include "lpc17xx.h"

volatile uint8_t button_zero = 0;
volatile uint8_t button_one = 0;
volatile uint8_t button_two = 0;

void EINT0_IRQHandler (void)	  	/* INT0														 */
{
	button_zero = 1;
	
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	NVIC_DisableIRQ(EINT0_IRQn);
	NVIC_DisableIRQ(EINT2_IRQn);
	
	LPC_PINCON->PINSEL4    &= ~(1 << 20);     /* GPIO pin selection */
	
  LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}

void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	button_one = 1;
	
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	NVIC_DisableIRQ(EINT0_IRQn);
	NVIC_DisableIRQ(EINT2_IRQn);
	
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */		// Apparently if I dont set the button's line to GPIO mode I won't be able to read it in the RIT handler
	
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */ 
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	button_two = 1;
	
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	NVIC_DisableIRQ(EINT0_IRQn);
	NVIC_DisableIRQ(EINT2_IRQn);
	
	LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection */
	
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}


