/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "button.h"
#include "RIT.h"
#include "QuoridorLib.h"
#include "gameGraphics.h"
#include "Joystick/joystick.h"

#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	
  LCD_Initialization();
	LCD_Clear(Black);
	//TP_Init();
	//TouchPanel_Calibrate_Default();
	
	game_setup();
	
	BUTTON_init();												/* BUTTON Initialization              */
	
	//init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       */
	joystick_init();											/* Joystick Initialization            */
	init_RIT(0x1312D0);									/* RIT Initialization 50 msec       	*/
	enable_RIT();													/* RIT enabled												*/
	
	init_timer(0, 0x1312D0 ); 						/* 50ms * 25MHz = 1.25*10^6 = 0x1312D0 */
	//init_timer(0, 0x6108 ); 						  /* 1ms * 25MHz = 25*10^3 = 0x6108 */
	//init_timer(0, 0x4E2 ); 						    /* 500us * 25MHz = 1.25*10^3 = 0x4E2 */
	//init_timer(0, 0xF ); 						    /* 8us * 25MHz = 200 ~= 0xC8 */
	
	init_timer(1, 0x1312D00 ); 		// used for fixing a bug for which the RICOUNTER may randomly go beyond its limit, missing the handler
	enable_timer(1);
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
