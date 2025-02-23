/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include "../QuoridorLib.h"
#include "../gameGraphics.h"
#include "../Joystick/joystick.h"

extern volatile Status game_status;

volatile Vector possible_moves; // contains 4 <GridCoord>s and the size of the vector [up to 4 of course] (how much of the data is actually valid)
extern volatile uint8_t phantom_created; // phantom_created indicates whether a move has been pre-selected or not
extern volatile GridCoords phantomPos;
extern uint8_t gamemode; // wall placement or player move

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER0_IRQHandler (void)
{
	static volatile int i, last_player = 0, ticks = 0, tps = 6, winner = 0; // tps = ticks per second
	
	// 0) detect turn/player change
	if (last_player != game_status.turn) {
		reset_disable_timeout_registration();
		// .. winning condition
		winner = is_win();
		if (winner) {
			show_winner_msg(winner);
			// disable timers
			disable_timer(0);
			// disable_RIT(); // per non fare casini con gli include "...h" faccio che farlo disabilitare al RIT stesso con check su game_status.win
			LPC_TIM0->IR = 1;			/* clear interrupt flag */
			return;
		}
		
		// a. reset turn loop data
		last_player = game_status.turn;
		game_status.remaining_time = TIMEOUT;
		ticks = 0;
		gamemode = PMOVE;
		updateInfometrics_turnWhoose();
		
		// b. clear map
		redraw_walls(game_status);
		if (phantom_created == 1) {
			phantom_created = 0;
			del_phantoms(possible_moves);
		}		
		gameInd_DrawGrid(7, 7);
		
		// c. get moves
		possible_moves = get_possible_moves(); // update possible_moves only at turn change
	}
	
	// 1) decrease 20s timer every x ticks => using my computer, this way it takes pretty much one exact second in simulation (with timer0 initialized with 0x1312D0 <-> 50ms)
	if (ticks++ == tps) {
		game_status.remaining_time--;
		updateInfometrics_time();
		ticks = 0;
	}	
	
	// 2) illuminate valid moves + character position, and deilluminate them alternately. Based on the 2) comment, I will turn it on and off every half of the total ticks for the second's duration.
	if (ticks == 1) {
		for (i = 0; i < possible_moves.size; i++)
			gameInd_IlluminateBox(possible_moves.vector[i].orr, possible_moves.vector[i].ver);
		gameInd_IlluminateBox(get_cur_player_pos().orr, get_cur_player_pos().ver);
	} else if (ticks == tps/2 + 1) {
		for (i = 0; i < possible_moves.size; i++)
			gameInd_deIlluminateBox(possible_moves.vector[i].orr, possible_moves.vector[i].ver);
		gameInd_deIlluminateBox(get_cur_player_pos().orr, get_cur_player_pos().ver);
	}
	
	// 3) if game_status.remaining_time == 0 then next turn
	if (game_status.remaining_time <= 0) {
		phantom_created;
		register_timeout_move();
		change_turn();		
	}
	
	// 4) if not then play
		
		
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void) // used for fixing a bug for which the RICOUNTER goes beyond its limit, missing the handler
{
	LPC_RIT->RICOUNTER = 0; 
	
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
