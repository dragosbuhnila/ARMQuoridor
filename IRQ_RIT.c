/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "timer/timer.h"

#include <stdio.h>
#include "button.h"

#include "QuoridorLib.h"
#include "gameGraphics.h"
#include "GLCD/GLCD.h" 


/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern volatile uint8_t button_zero;
extern volatile uint8_t button_one;
extern volatile uint8_t button_two;

volatile uint8_t gamemode = PMOVE;

extern Status game_status;
extern Vector possible_moves; // Contains up to 4 moves
volatile uint8_t phantom_created=0; // phantom_created indicates whether a move has been pre-selected or not
volatile GridCoords phantomPos;
volatile GridCoords wall_pos; // wall placement or player move
volatile uint8_t wall_orientation = Vertical;

void RIT_IRQHandler (void) {			
	static int8_t up=0, down=0, left=0, right=0, select=0, placed = 0, must_clean = 0;
	GridCoords cur_pos = get_cur_player_pos(), jump_pos;
	
	if (game_status.win == 1) {
		NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
		NVIC_DisableIRQ(EINT0_IRQn);
		NVIC_DisableIRQ(EINT2_IRQn);
		
		button_zero = 0;
		button_one = 0;
		button_two = 0;
		
		LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		
		reset_RIT();
		LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
		return;
	}
	
	/* ===================== Joystick. We don't have IRQ_Handlers for it so we must initiate a polling procedure. ===================== */
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){
		select++;
		switch(select){
			case 1:
				// code for !!!!!JoySELECT!!!!!
				if (!game_status.started)
						break;

				if (gamemode == PMOVE) {
					if (phantom_created == 1) { // phantom_created means that a move has been pre-selected effectively
						gameInd_DelPG(cur_pos.orr, cur_pos.ver);
						
						if (game_status.turn == 1) {
							game_status.pos_p1 = phantomPos;
							showP1();
						} else if (game_status.turn == 2) {
							game_status.pos_p2 = phantomPos;
							showP2();
						}
						
						register_player_move();
						phantom_created = 0;
						game_status.remaining_time = 0;
					}
				} else if (gamemode == WALLMOVE) {
					if (must_clean == 1) {
						must_clean = 0;
						clean_walls_msg();
						break;
					}
					
					placed = try_place_wall(wall_pos.orr, wall_pos.ver, wall_orientation); // it automatically updates the table by itself
					if (placed == 2)									// I had to split the function in two because of FULLMEM or STACK OVERFLOW errors. not sure which.
						placed = try_place_wall_checkopp(wall_pos.orr, wall_pos.ver, wall_orientation);
					
					if (placed == -2) { // means player doesn't have walls
						must_clean = 1;
						show_walls_msg();
					}
					
					if (placed == 1) {
						if (game_status.turn == 1) {
							game_status.walls_remaining_p1--;
							updateInfometrics_wallsP1();
						} else if (game_status.turn == 2) {
							game_status.walls_remaining_p2--;
							updateInfometrics_wallsP2();
						}
						
					register_wall_move(wall_pos.orr, wall_pos.ver, wall_orientation);
					game_status.remaining_time = 0;
					}
				}
				
				
				break;
			default:
				break;
		}
	}	else { select=0;}
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){
		down++;
		switch(down){
			case 1:
				// code for !!!!!JoyDOWN!!!!!
				if (!game_status.started)
						break;
			
				
				if (gamemode == PMOVE) {					
					cur_pos.ver++;
					
					jump_pos.ver = cur_pos.ver + 1;
					jump_pos.orr = cur_pos.orr;
					if (move_is_valid(cur_pos)) {
						phantomPos = cur_pos;
						phantom_created = 1;
						
						del_phantoms(possible_moves);
						showPhantom(phantomPos);
					} else if (move_is_valid(jump_pos)) {
						phantomPos = jump_pos;
						phantom_created = 1;
						
						del_phantoms(possible_moves);
						showPhantom(phantomPos);
					} else {
						phantom_created = 0;
					}
					
				} else if (gamemode == WALLMOVE) {
					if (wall_pos.ver + 1 > 5)
						break;
					
					wall_pos.ver++;
					redraw_walls(game_status);
					gameInd_DrawTempWall(wall_pos.orr, wall_pos.ver, wall_orientation);
				}
			
			
			
				break;
			default:
				break;
		}
	}	else { down=0;}
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){
		left++;
		switch(left){
			case 1:
				// code for !!!!!JoyLEFT!!!!!
				if (!game_status.started)
						break;
			
				
				if (gamemode == PMOVE) {					
					cur_pos.orr--;
					
					jump_pos.ver = cur_pos.ver;
					jump_pos.orr = cur_pos.orr - 1;
					if (move_is_valid(cur_pos)) {
						phantomPos = cur_pos;
						phantom_created = 1;
						
						del_phantoms(possible_moves);
						showPhantom(phantomPos);
					} else if (move_is_valid(jump_pos)) {
						phantomPos = jump_pos;
						phantom_created = 1;
						
						del_phantoms(possible_moves);
						showPhantom(phantomPos);
					} else {
						del_phantoms(possible_moves);
						phantom_created = 0;
					}
					
				} else if (gamemode == WALLMOVE) {
						if (wall_pos.orr - 1 < 0)
							break;
					
					wall_pos.orr--;
					redraw_walls(game_status);
					gameInd_DrawTempWall(wall_pos.orr, wall_pos.ver, wall_orientation);
				}
			
			
			
				break;
			default:
				break;
		}
	}	else { left=0;}
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){
		right++;
		switch(right){
			case 1:
				// code for !!!!!JoyRIGHT!!!!!
				if (!game_status.started)
						break;
			
				if (gamemode == PMOVE) {					
					cur_pos.orr++;
					
					jump_pos.ver = cur_pos.ver;
					jump_pos.orr = cur_pos.orr+1;
					if (move_is_valid(cur_pos)) {
						phantomPos = cur_pos;
						phantom_created = 1;
						
						del_phantoms(possible_moves);
						showPhantom(phantomPos);
					} else if (move_is_valid(jump_pos)) {
						phantomPos = jump_pos;
						phantom_created = 1;
						
						del_phantoms(possible_moves);
						showPhantom(phantomPos);
					} else {
						phantom_created = 0;
					}
					
				} else if (gamemode == WALLMOVE) {
					if (wall_pos.orr + 1 > 5)
						break;
					
					wall_pos.orr++;
					redraw_walls(game_status);
					gameInd_DrawTempWall(wall_pos.orr, wall_pos.ver, wall_orientation);
				}
			
				break;
			default:
				break;
		}
	}	else { right=0;}
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		up++;
		switch(up){
			case 1:
				// code for !!!!!JoyUP!!!!!
				if (!game_status.started)
						break;
			
				
				if (gamemode == PMOVE) {					
					cur_pos.ver--;
					
					jump_pos.ver = cur_pos.ver - 1;
					jump_pos.orr = cur_pos.orr;
					if (move_is_valid(cur_pos)) {
						phantomPos = cur_pos;
						phantom_created = 1;
						
						del_phantoms(possible_moves);
						showPhantom(phantomPos);
					} else if (move_is_valid(jump_pos)) {
						phantomPos = jump_pos;
						phantom_created = 1;
						
						del_phantoms(possible_moves);
						showPhantom(phantomPos);
					} else {
						phantom_created = 0;
					}
					
				} else if (gamemode == WALLMOVE) {
					if (wall_pos.ver - 1 < 0)
						break;
					
					wall_pos.ver--;
					redraw_walls(game_status);
					gameInd_DrawTempWall(wall_pos.orr, wall_pos.ver, wall_orientation);
				}
			
			
			
				break;
			default:
				break;
		}
	}	else { up=0;}
	/* ===================== End of Joystick polling procedure =====================*/	
	
	/* ===================== Button pressing manager =====================*/
	if ( (LPC_GPIO2->FIOPIN & (1<<11)) == 0 && button_one != 0) {
			switch(button_one){
				case 2:
					// code for !!!!!BUTTON _1_!!!!!
					if (game_status.started == 0)
						break;
				
					if (gamemode == PMOVE)
						gamemode = WALLMOVE;
					else if (gamemode == WALLMOVE)
						gamemode = PMOVE;
					
					if (gamemode == WALLMOVE) {
						
						wall_pos.orr = 2;
						wall_pos.ver = 3;
						wall_orientation = Vertical;
						draw_wall_in_centre();
					}
					
					if (gamemode == PMOVE) {
						redraw_walls(game_status);
					}
				
					break;
				default:
					break;
			}
			button_one++;
	} else if	(button_one == 1) {
		button_one++;
	}	else if ( (LPC_GPIO2->FIOPIN & (1<<12)) == 0 && button_two != 0) {
			switch (button_two) {
				case 2: 
					
				// code for !!!!!BUTTON _2_!!!!!
					if (gamemode == WALLMOVE) {
						wall_orientation = (wall_orientation + 1) % 2;
						redraw_walls(game_status);
						gameInd_DrawTempWall(wall_pos.orr, wall_pos.ver, wall_orientation);
					}
				
					break;
				default:
						break;
			}
			button_two++;
	}	else if (button_two == 1) {
		button_two++;
	} else	if ( (LPC_GPIO2->FIOPIN & (1<<10)) == 0 && button_zero != 0) {
			switch (button_zero) {
				case 2: 
					
					// code for !!!!!BUTTON _0_!!!!!
					game_status.started = 1;
					showP1();
					showP2();
					updateInfometrics_turnWhoose();
					enable_timer(0); // for both highlighting and 20s count
				
					break;
				default:
						break;
			}
			button_zero++;
	} else if (button_zero == 1) {
		button_zero++;
	}	else {							/* button released */		
		NVIC_EnableIRQ(EINT0_IRQn);	
		NVIC_EnableIRQ(EINT1_IRQn);
		NVIC_EnableIRQ(EINT2_IRQn);
		
		button_zero = 0;
		button_one = 0;
		button_two = 0;
		
		LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
	}
	/* ===================== End of button pressing manager =====================*/
	
	reset_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
