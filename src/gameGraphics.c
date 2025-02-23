#include <stdio.h>
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "QuoridorLib.h"
#include "gameGraphics.h"


void gameCoord_DrawBox(uint16_t X, uint16_t Y, uint16_t color) { // from the starting point (x,y), it will draw a square extending in the bottom-left direction
	int i = 0;
	uint16_t X1, X2, Y1, Y2;
	
	X1 = X + 3;
	X2 = X + 34 - 3;
	Y1 = Y + 3;
	Y2 = Y + 34 - 3;
	
	for (i = 0; i < 2; i++) {
		LCD_DrawLine(X1, Y1 + i, X2, Y1 + i, color); // horizontal 1
		LCD_DrawLine(X1 + i, Y1, X1 + i, Y2, color); // vertical 1
		LCD_DrawLine(X1, Y2 - i, X2, Y2 - i, color);	// horizontal 2
		LCD_DrawLine(X2 - i, Y1, X2 - i, Y2, color); // vertical 2
	}
}

void gameCoord_DrawWall(uint16_t X, uint16_t Y, int orientation) { // if horizontal==0 => vertical, if horizontal==1 => horizontal
	int i = 0;
	uint16_t wall_color = Red;
	
	if (orientation == Horizontal)
		for (i = 0; i < 3; i++)
			LCD_DrawLine(X-32, Y+i, X+33, Y+i, wall_color); // making it just two pixels short on each edge: instead of X-34_X+35 we have X-32_X-33
	else
		for (i = 0; i < 3; i++)
			LCD_DrawLine(X+i, Y - 32, X+i, Y + 33, wall_color);
}

void gameCoord_DrawGrid(uint16_t sizeX, uint16_t sizeY) {
	int i, j;
	for (i = 0; i < sizeX; i++)
		for (j = 0; j < sizeY; j++)
			gameCoord_DrawBox(i*34, j*34, White);
}

void gameInd_DrawBox(uint16_t indexOrr, uint16_t indexVert, uint16_t color) {
	if (indexOrr > 6)
		return; // out of bounds horizontally
	if (indexVert > 6)
		return; // out of bounds vertically
	gameCoord_DrawBox(indexOrr*34, indexVert*34, color);
}

void gameInd_DrawWall(uint16_t indexOrr, uint16_t indexVert, int horizontal) { // if horizontal==0 => vertical, if horizontal==1 => horizontal
	if (indexOrr > 5)
		return; // out of bounds horizontally
	if (indexVert > 5)
		return; // out of bounds vertically
	gameCoord_DrawWall(34 + indexOrr*34, 34+ indexVert*34, horizontal);
}

void gameInd_DrawWall_choosecolor(uint16_t indexOrr, uint16_t indexVert, int orientation, uint16_t color) {
	int i = 0, X, Y;
	uint16_t wall_color = color;
	
	if (indexOrr > 5)
		return; // out of bounds horizontally
	if (indexVert > 5)
		return; // out of bounds vertically
	X = 34 + indexOrr*34;
	Y = 34+ indexVert*34;
	
	if (orientation == Horizontal)
		for (i = 0; i < 3; i++)
			LCD_DrawLine(X-32, Y+i, X+33, Y+i, wall_color); // making it just two pixels short on each edge: instead of X-34_X+35 we have X-32_X-33
	else
		for (i = 0; i < 3; i++)
			LCD_DrawLine(X+i, Y - 32, X+i, Y + 33, wall_color);
}

void gameInd_DrawTempWall(uint16_t indexOrr, uint16_t indexVert, int orientation) {
	int X, Y;
	uint16_t wall_color = Yellow;
	
	if (indexOrr > 5)
		return; // out of bounds horizontally
	if (indexVert > 5)
		return; // out of bounds vertically
	X = 34 + indexOrr*34;
	Y = 34+ indexVert*34;
	
	if (orientation == Horizontal) {
		LCD_DrawLine(X-29, Y, X+30, Y, wall_color);
		LCD_DrawLine(X-29, Y+1, X+30, Y+1, wall_color);
	}	else if (orientation == Vertical) {
		LCD_DrawLine(X, Y - 29, X, Y + 30, wall_color);
		LCD_DrawLine(X+1, Y - 29, X+1, Y + 30, wall_color);
	}
}

void gameInd_DrawGrid(uint16_t sizeX, uint16_t sizeY) {
	int i, j;
	for (i = 0; i < sizeX; i++)
		for (j = 0; j < sizeY; j++)
			gameInd_DrawBox(i, j, White);
}

void gameInd_IlluminateBox_ChooseColor(uint16_t indexOrr, uint16_t indexVert, uint16_t color) {
	gameInd_DrawBox(indexOrr, indexVert, color);
}
	
void gameInd_IlluminateBox(uint16_t indexOrr, uint16_t indexVert) {
	gameInd_IlluminateBox_ChooseColor(indexOrr, indexVert, Yellow);
}

void gameInd_deIlluminateBox(uint16_t indexOrr, uint16_t indexVert) {
	gameInd_IlluminateBox_ChooseColor(indexOrr, indexVert, White);
}

void gameCoord_DrawPG(uint8_t X, uint8_t Y, uint16_t color) {
	PutChar(X+13, Y+11, '@', color, Black);
}

void gameInd_DrawPG(uint8_t indexOrr, uint8_t indexVert, uint16_t color) {
	gameCoord_DrawPG(indexOrr*34, indexVert*34, color);
}

void gameInd_DelPG(uint8_t indexOrr, uint8_t indexVert) {
	int X1, X2, Y, i;
	X1 = indexOrr*34 + 10;
	Y = indexVert*34 + 10;
	X2 = indexOrr*34 + 34 - 10;
	for (i = 0; i < 14; i++)
		LCD_DrawLine(X1, Y + i, X2, Y + i, Black);
}

void gameCoord_DrawBoxBigger(uint16_t X, uint16_t Y, uint16_t color) { // from the starting point (x,y), it will draw a square extending in the bottom-left direction
	int i = 0;
	uint16_t X1, X2, Y1, Y2;
	
	X1 = X + 3;
	X2 = X + 80 - 3;
	Y1 = Y + 3;
	Y2 = Y + 80 - 3;
	
	for (i = 0; i < 3; i++) {
		LCD_DrawLine(X1, Y1 + i, X2, Y1 + i, color); // horizontal 1
		LCD_DrawLine(X1 + i, Y1, X1 + i, Y2, color); // vertical 1
		LCD_DrawLine(X1, Y2 - i, X2, Y2 - i, color);	// horizontal 2
		LCD_DrawLine(X2 - i, Y1, X2 - i, Y2, color); // vertical 2
	}
}

void game_CleanTime() { // 105, 280,
	int X1, X2, Y, i;
	X1 = 105;
	Y = 280;
	X2 = 140;
	for (i = 0; i < 14; i++)
	LCD_DrawLine(X1, Y + i, X2, Y + i, Black);
}

void draw_wall_in_centre(void) {
	gameInd_DrawTempWall(2, 3, Vertical);
}

void redraw_walls(Status game_status) {
	int i, j;
	
	for (i = 0; i < 6; i++)
		for (j = 0; j < 6; j++) {
			gameInd_DrawWall_choosecolor(i, j, Horizontal, Black);
			gameInd_DrawWall_choosecolor(i, j, Vertical, Black);
		}
	
	for (i = 0; i < 6; i++)
		for (j = 0; j < 6; j++) {
			if (game_status.center_of_walls_table[i][j] == Horizontal)
				gameInd_DrawWall(i, j, Horizontal);
			else if (game_status.center_of_walls_table[i][j] == Vertical)
				gameInd_DrawWall(i, j, Vertical);
		}
}

void del_phantoms(Vector possible_moves) {
	int i;
	
	for (i = 0; i < possible_moves.size; i++) {
		gameInd_DelPG(possible_moves.vector[i].orr, possible_moves.vector[i].ver);
	}
}

void show_winner_msg(int player) {
	char win_msg[15];
	
	sprintf(win_msg, "Player %d wins!", player);
	GUI_Text(34*2, 34*4, win_msg, Black, Yellow);
}

void show_walls_msg(void) {
	GUI_Text(88, 295, "No walls", Black, Yellow);
}

void clean_walls_msg(void) {
	int i ;
	for (i = 0; i < 16; i++) 
		LCD_DrawLine(88, 295+i, 152, 295+i, Black);
}


void redraw_all(Status game_status) { // always crashes bc of memory
	LCD_Clear(Black);
	gameInd_DrawGrid(7, 7);
	
	gameCoord_DrawBoxBigger(0, 240, White);
	gameCoord_DrawBoxBigger(80, 240, White);
	gameCoord_DrawBoxBigger(160, 240, White);	
	
	updateInfometrics_wallsP1();
	updateInfometrics_wallsP2();
	updateInfometrics_time();
	updateInfometrics_turnWhoose();
	
	showP1();
	showP2();
	
	redraw_walls(game_status);
}


