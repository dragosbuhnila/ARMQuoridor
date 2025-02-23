#include <stdio.h>
#include <stdbool.h>
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "QuoridorLib.h"
#include "gameGraphics.h"
#include "Joystick/joystick.h"
//#include "Graph/Graph.h" included in the QuoridorLib.h

Status game_status;
extern volatile Vector possible_moves;
volatile uint8_t disable_timeout_registration = 0;

int GridCoords_to_integer_pos (GridCoords GridCoords_pos) {
	return GridCoords_pos.orr*10 + GridCoords_pos.ver;
}

GridCoords integer_to_GridCoords_pos(int integer_pos) {
	GridCoords GridCoords_pos;
	GridCoords_pos.orr = integer_pos/10;
	GridCoords_pos.ver = integer_pos%10;
	return GridCoords_pos;
}

void game_setup() {
	int x, y;
	
	game_status.walls_remaining_p1 = 8;
	game_status.walls_remaining_p2 = 8;
	
	game_status.xi_max = 6;
	game_status.yi_max = 6;
	game_status.win = 0;
	game_status.turn = 1;
	game_status.started = 0;
	
	// starting positions set
	game_status.pos_p1.orr = 3;
	game_status.pos_p1.ver = 6;
	game_status.pos_p2.orr = 3;
	game_status.pos_p2.ver = 0;
	
	game_status.remaining_time = TIMEOUT; // use values lower than 20 for debugging
	
	// filling walls tables with -1s
	for (x = 0; x < 6; x++)
		for (y = 0; y < 6; y++)
			game_status.center_of_walls_table[x][y] = -1; // -1 means not present. 0 (Vertical), 1 (Horizontal)
	for(x = 0; x < 6; x++)
		for(y = 0; y < 7; y++)
			game_status.vertical_walls[x][y] = -1; // -1 means not present
	for(x = 0; x < 7; x++)
		for(y = 0; y < 6; y++)
			game_status.horizontal_walls[x][y] = -1; // -1 means not present
		
	gameInd_DrawGrid(7, 7);
	gameCoord_DrawBoxBigger(0, 240, White);
	gameCoord_DrawBoxBigger(80, 240, White);
	gameCoord_DrawBoxBigger(160, 240, White);	
	
	updateInfometrics_wallsP1();
	updateInfometrics_wallsP2();
	updateInfometrics_time();
}

void showP1() {
	//gameInd_DrawPG(game_status.pos_p1.orr, game_status.pos_p1.ver, Red);
	int X = game_status.pos_p1.orr*34, Y = game_status.pos_p1.ver*34;
	PutChar(X+13, Y+11, '@', Red, Black);
}

void showPhantom(GridCoords pos) {
	gameInd_DrawPG(pos.orr, pos.ver, Grey);
}

void showP2() {
	gameInd_DrawPG(game_status.pos_p2.orr, game_status.pos_p2.ver, Blue2);
}

void updateInfometrics_wallsP1(){
	char walls1[15];
	
	GUI_Text(8, 255, "Player 1", White, Black);
	sprintf(walls1, "Walls: %d", game_status.walls_remaining_p1);
	GUI_Text(8, 280, walls1, White, Black);
}

void updateInfometrics_wallsP2(){
	char walls2[15];
	
	GUI_Text(168, 255, "Player 2", White, Black);
	sprintf(walls2, "Walls: %d", game_status.walls_remaining_p2);
	GUI_Text(168, 280, walls2, White, Black);
}

void updateInfometrics_turnWhoose(){
	char turnWhoose[15];
	
	sprintf(turnWhoose, "P%d Turn", game_status.turn);
	GUI_Text(92, 255, turnWhoose, White, Black);	
}

void updateInfometrics_time(){
	char remtime[15];
	
	sprintf(remtime, "%d s", game_status.remaining_time);
	game_CleanTime();
	GUI_Text(105, 280, remtime, White, Black);	
}

Vector get_possible_moves_bypos(GridCoords specified_position) {
	int8_t orr, ver, i = 0, occ_by_other_player, double_check;
	
	GridCoords cur_pos = specified_position, opposite_player_pos = get_opp_player_pos();
	Surrounding_Walls surrounding_walls = get_surrounding_walls(cur_pos.orr, cur_pos.ver);
	static Vector possible_moves_loc;
	
	/* check for LEFT */
	orr = cur_pos.orr - 1;
	ver = cur_pos.ver;
	
	double_check = 1;
	occ_by_other_player = 0;
	if (opposite_player_pos.orr == orr && opposite_player_pos.ver == ver)
		occ_by_other_player = 0;
	
	if (!(orr < 0) && surrounding_walls.left == -1) {
		if (occ_by_other_player) {
			if (!(orr - 1 < 0))
				orr--;
			else
				double_check = 0;
		}
		if (double_check == 1) {
			possible_moves_loc.vector[i].orr = orr;
			possible_moves_loc.vector[i++].ver = ver;
		}
	}
		
	/* check for TOP */
	orr = cur_pos.orr;
	ver = cur_pos.ver - 1;
	
	double_check = 1;
	occ_by_other_player = 0;
	if (opposite_player_pos.orr == orr && opposite_player_pos.ver == ver)
		occ_by_other_player = 0;
	
	if (!(ver < 0) && surrounding_walls.up == -1) {
		if (occ_by_other_player) {
			if (!(ver - 1 < 0))
				ver--;
			else
				double_check = 0;
		}
		if (double_check == 1) {
			possible_moves_loc.vector[i].orr = orr;
			possible_moves_loc.vector[i++].ver = ver;
		}
	}
	
	/* check for RIGHT */
	orr = cur_pos.orr + 1;
	ver = cur_pos.ver;
	
	double_check = 1;
	occ_by_other_player = 0;
	if (opposite_player_pos.orr == orr && opposite_player_pos.ver == ver)
		occ_by_other_player = 0;
	
	if (!(orr > game_status.xi_max) && surrounding_walls.right == -1) {
		if (occ_by_other_player) {
			if (!(orr + 1 > game_status.xi_max))
				orr++;
			else
				double_check = 0;
		}
		if (double_check == 1) {
			possible_moves_loc.vector[i].orr = orr;
			possible_moves_loc.vector[i++].ver = ver;
		}
	}
	
	/* check for BOTTOM */
	orr = cur_pos.orr;
	ver = cur_pos.ver + 1;
	
	double_check = 1;
	occ_by_other_player = 0;
	if (opposite_player_pos.orr == orr && opposite_player_pos.ver == ver)
		occ_by_other_player = 0;
	
	if (!(ver > game_status.xi_max) && surrounding_walls.bot == -1) {
		if (occ_by_other_player) {
			if (!(ver + 1 > game_status.xi_max))
				ver++;
			else
				double_check = 0;
		}
		if (double_check == 1) {
			possible_moves_loc.vector[i].orr = orr;
			possible_moves_loc.vector[i++].ver = ver;
		}
	}
	
	possible_moves_loc.size = i;
	
	return possible_moves_loc;
}

Vector get_possible_moves() {
	int8_t orr, ver, i = 0, occ_by_other_player, double_check;
	
	GridCoords cur_pos = get_cur_player_pos(), opposite_player_pos = get_opp_player_pos();
	Surrounding_Walls surrounding_walls = get_surrounding_walls(cur_pos.orr, cur_pos.ver);
	static Vector possible_moves_loc;
	
	/* check for LEFT */
	orr = cur_pos.orr - 1;
	ver = cur_pos.ver;
	
	double_check = 1;
	occ_by_other_player = 0;
	if (opposite_player_pos.orr == orr && opposite_player_pos.ver == ver)
		occ_by_other_player = 1;
	
	if (!(orr < 0) && surrounding_walls.left == -1) {
		if (occ_by_other_player) {
			if (!(orr - 1 < 0))
				orr--;
			else
				double_check = 0;
		}
		if (double_check == 1) {
			possible_moves_loc.vector[i].orr = orr;
			possible_moves_loc.vector[i++].ver = ver;
		}
	}
		
	/* check for TOP */
	orr = cur_pos.orr;
	ver = cur_pos.ver - 1;
	
	double_check = 1;
	occ_by_other_player = 0;
	if (opposite_player_pos.orr == orr && opposite_player_pos.ver == ver)
		occ_by_other_player = 1;
	
	if (!(ver < 0) && surrounding_walls.up == -1) {
		if (occ_by_other_player) {
			if (!(ver - 1 < 0))
				ver--;
			else
				double_check = 0;
		}
		if (double_check == 1) {
			possible_moves_loc.vector[i].orr = orr;
			possible_moves_loc.vector[i++].ver = ver;
		}
	}
	
	/* check for RIGHT */
	orr = cur_pos.orr + 1;
	ver = cur_pos.ver;
	
	double_check = 1;
	occ_by_other_player = 0;
	if (opposite_player_pos.orr == orr && opposite_player_pos.ver == ver)
		occ_by_other_player = 1;
	
	if (!(orr > game_status.xi_max) && surrounding_walls.right == -1) {
		if (occ_by_other_player) {
			if (!(orr + 1 > game_status.xi_max))
				orr++;
			else
				double_check = 0;
		}
		if (double_check == 1) {
			possible_moves_loc.vector[i].orr = orr;
			possible_moves_loc.vector[i++].ver = ver;
		}
	}
	
	/* check for BOTTOM */
	orr = cur_pos.orr;
	ver = cur_pos.ver + 1;
	
	double_check = 1;
	occ_by_other_player = 0;
	if (opposite_player_pos.orr == orr && opposite_player_pos.ver == ver)
		occ_by_other_player = 1;
	
	if (!(ver > game_status.xi_max) && surrounding_walls.bot == -1) {
		if (occ_by_other_player) {
			if (!(ver + 1 > game_status.xi_max))
				ver++;
			else
				double_check = 0;
		}
		if (double_check == 1) {
			possible_moves_loc.vector[i].orr = orr;
			possible_moves_loc.vector[i++].ver = ver;
		}
	}
	
	possible_moves_loc.size = i;
	
	return possible_moves_loc;
}

GridCoords get_cur_player_pos() {
	GridCoords spare;
	spare.ver = -3;
	spare.orr = -3;
	
	if (game_status.turn == 1)
		return game_status.pos_p1;
	if (game_status.turn == 2)
		return game_status.pos_p2;
	
	return spare;
}

GridCoords get_opp_player_pos(void) {
	if (game_status.turn == 2)
		return game_status.pos_p1;
	else
		return game_status.pos_p2;
}

void change_turn() {
	if (game_status.turn == 1)
		game_status.turn = 2;
	else if (game_status.turn == 2)
		game_status.turn = 1;
	else
		game_status.turn = 0;
}

/* Returns 1 if successfull, 0 if not*/
int try_place_wall(uint8_t X, uint8_t Y, uint8_t orientation) {
	uint8_t i, j, tail, head, pos_visited, cur_player_okay;
	GridCoords tile1, tile2, cur_pos; // a tile is a half wall
	static GridCoords queue[47];
	static int8_t visited[7][7]; // 1 if visited, -1 if queued, 0 if not visited nor queued yet
	Vector possible_moves; 
	
	// Check_0) walls available?
	if (game_status.turn == 1) {
		if (game_status.walls_remaining_p1 <= 0) {
			show_walls_msg();
			return -2;
		}
	}	else if (game_status.turn == 2) {
		if (game_status.walls_remaining_p2 <= 0) {
			return 0;
		}
	}
	
	// Check_1) colliding (center) walls in different directions
	if (game_status.center_of_walls_table[X][Y] != -1)
		return 0;
	
	// Check_2) colliding walls in same direction
	if (orientation == Horizontal) {
		tile1.orr = X;
		tile1.ver = Y;
		tile2.orr = X + 1;
		tile2.ver = Y;
		if (game_status.horizontal_walls[tile1.orr][tile1.ver] == 1 || game_status.horizontal_walls[tile2.orr][tile2.ver] == 1)
			return 0;
	} else if (orientation == Vertical) {
		tile1.orr = X;
		tile1.ver = Y;
		tile2.orr = X;
		tile2.ver = Y + 1;
		if (game_status.vertical_walls[tile1.orr][tile1.ver] == 1 || game_status.vertical_walls[tile2.orr][tile2.ver] == 1)
			return 0;
	}
	
	// preliminarily upating the the walls tables
	if (orientation == Horizontal) {
		game_status.horizontal_walls[tile1.orr][tile1.ver] = 1;
		game_status.horizontal_walls[tile2.orr][tile2.ver] = 1;
		game_status.center_of_walls_table[X][Y] = Horizontal;
	} else if (orientation == Vertical) {
		game_status.vertical_walls[tile1.orr][tile1.ver] = 1;
		game_status.vertical_walls[tile2.orr][tile2.ver] = 1;
		game_status.center_of_walls_table[X][Y] = Vertical;
	}
	
	// Check_3) route obstruction
	// for current player 
	cur_pos = get_cur_player_pos();	
	for (i = 0; i < 7; i++)
		for (j = 0; j < 7; j++)
			visited[i][j] = 0;
	visited[cur_pos.orr][cur_pos.ver] = 1;
	
	tail=0;
	head=0;
	possible_moves = get_possible_moves_bypos(cur_pos);
	for (i = 0; i < possible_moves.size; i++)
		queue[tail++] = possible_moves.vector[i];
	
	cur_player_okay = 0;
	while (head < tail) { // minore stretto perchè tail è tail++, e dunque punta alla prima casella libera, non all'ultima casella occupata
		cur_pos = queue[head++];
		if (is_winning_position(cur_pos, 1)) {
			cur_player_okay = 1;
			break;
		}
		
		visited[cur_pos.orr][cur_pos.ver] = 1;
		possible_moves = get_possible_moves_bypos(cur_pos);
		for (i = 0; i < possible_moves.size; i++) {
			pos_visited = visited[possible_moves.vector[i].orr][possible_moves.vector[i].ver];
			if (pos_visited == 0) { // meaning, not visited nor already queued
				visited[possible_moves.vector[i].orr][possible_moves.vector[i].ver] = -1;
				queue[tail++] = possible_moves.vector[i];
			}
		}
	}
	
	// reset the tables
	if (orientation == Horizontal) {
		game_status.horizontal_walls[tile1.orr][tile1.ver] = -1;
		game_status.horizontal_walls[tile2.orr][tile2.ver] = -1;
		game_status.center_of_walls_table[X][Y] = -1;
	} else if (orientation == Vertical) {
		game_status.vertical_walls[tile1.orr][tile1.ver] = -1;
		game_status.vertical_walls[tile2.orr][tile2.ver] = -1;
		game_status.center_of_walls_table[X][Y] = -1;
	}
	
	if (cur_player_okay == 1) {
		return 2;
	} else {
		return 0;
	}
}

int try_place_wall_checkopp(uint8_t X, uint8_t Y, uint8_t orientation) {
	uint8_t i, j, tail, head, pos_visited;
	GridCoords tile1, tile2, cur_pos; // a tile is a half wall
	static GridCoords queue[47];
	static int8_t visited[7][7]; // 1 if visited, -1 if queued, 0 if not visited nor queued yet
	Vector possible_moves; 
	
	if (orientation == Horizontal) {
		tile1.orr = X;
		tile1.ver = Y;
		tile2.orr = X + 1;
		tile2.ver = Y;
	} else if (orientation == Vertical) {
		tile1.orr = X;
		tile1.ver = Y;
		tile2.orr = X;
		tile2.ver = Y + 1;
	}
	
	// preliminarily upating the the walls tables
	if (orientation == Horizontal) {
		game_status.horizontal_walls[tile1.orr][tile1.ver] = 1;
		game_status.horizontal_walls[tile2.orr][tile2.ver] = 1;
		game_status.center_of_walls_table[X][Y] = Horizontal;
	} else if (orientation == Vertical) {
		game_status.vertical_walls[tile1.orr][tile1.ver] = 1;
		game_status.vertical_walls[tile2.orr][tile2.ver] = 1;
		game_status.center_of_walls_table[X][Y] = Vertical;
	}
	
	// opposite player propagation
	cur_pos = get_opp_player_pos();	
	for (i = 0; i < 7; i++)
		for (j = 0; j < 7; j++)
			visited[i][j] = 0;
	visited[cur_pos.orr][cur_pos.ver] = 1;
	
	tail=0;
	head=0;
	possible_moves = get_possible_moves_bypos(cur_pos);
	for (i = 0; i < possible_moves.size; i++) {
		queue[tail] = possible_moves.vector[i];
		tail = tail + 1;
	}
	cur_pos = queue[head];
	
	while (head < tail) { // minore stretto perchè tail è tail++, e dunque punta alla prima casella libera, non all'ultima casella occupata
		cur_pos = queue[head++];
		if (is_winning_position(cur_pos, 2))
			return 1; // 1 means propagation was succefull for both cur and opp players
		
		visited[cur_pos.orr][cur_pos.ver] = 1;
		possible_moves = get_possible_moves_bypos(cur_pos);
		for (i = 0; i < possible_moves.size; i++) {
			pos_visited = visited[possible_moves.vector[i].orr][possible_moves.vector[i].ver];
			if (pos_visited == 0) { // meaning, not visited nor already queued
				visited[possible_moves.vector[i].orr][possible_moves.vector[i].ver] = -1;
				queue[tail++] = possible_moves.vector[i];
			}
		}
	}
	
	// reset the tables in case there were obstructions (i.e. we have not returned yet)
	if (orientation == Horizontal) {
		game_status.horizontal_walls[tile1.orr][tile1.ver] = -1;
		game_status.horizontal_walls[tile2.orr][tile2.ver] = -1;
		game_status.center_of_walls_table[X][Y] = -1;
	} else if (orientation == Vertical) {
		game_status.vertical_walls[tile1.orr][tile1.ver] = -1;
		game_status.vertical_walls[tile2.orr][tile2.ver] = -1;
		game_status.center_of_walls_table[X][Y] = -1;
	}
	return 0;
}

Surrounding_Walls get_surrounding_walls(uint8_t X, uint8_t Y) { // the input coordinates are the player grid, not wall grids
	Surrounding_Walls surrounding_walls;
	GridCoords up, bot, left, right; // up and bot are horizontal, left and right are vertical
	surrounding_walls.left = -1;
	surrounding_walls.up = -1;
	surrounding_walls.right = -1;
	surrounding_walls.bot = -1;
	
	up.orr = X;
	up.ver = Y - 1;
	
	bot.orr = X;
	bot.ver = Y;
	
	left.orr = X - 1;
	left.ver = Y;
	
	right.orr = X;
	right.ver = Y;
	
	if (left.orr < 0) // -1 is the init value so it means no walls. 0 means border. 1 means wall
		surrounding_walls.left = 0;
	else
		surrounding_walls.left = game_status.vertical_walls[left.orr][left.ver];
	
	if (right.orr > 5)
		surrounding_walls.right = 0;
	else
		surrounding_walls.right = game_status.vertical_walls[right.orr][right.ver];
	
	if (up.ver < 0)
		surrounding_walls.up = 0;
	else
		surrounding_walls.up = game_status.horizontal_walls[up.orr][up.ver];
	
	if (bot.ver > 5)
		surrounding_walls.bot = 0;
	else
		surrounding_walls.bot = game_status.horizontal_walls[bot.orr][bot.ver];
	
	return surrounding_walls;
}

int* get_wall_blocked_positions(GridCoords tile, int orientation) {
	static int blocked_positions[2];
	int x = tile.orr, y = tile.ver;
	
	if (orientation == Horizontal) {
		blocked_positions[0] = x*10 + y;
		blocked_positions[1] = x*10 + y+1;
	} else if (orientation == Vertical) {
		blocked_positions[0] = x*10 + y;
		blocked_positions[1] = (x+1)*10 + y;;
	}
		
	return blocked_positions;
}


Vector centerWall_to_tilesWall (uint8_t X, uint8_t Y, uint8_t orientation) {
	GridCoords tile1, tile2; // a tile is a half wall
	Vector tiles;
	tiles.size = 2;
	
	if (orientation == Horizontal) {
		tile1.orr = X;
		tile1.ver = Y;
		tile2.orr = X + 1;
		tile2.ver = Y;
	} else if (orientation == Vertical) {
		tile1.orr = X;
		tile1.ver = Y;
		tile2.orr = X;
		tile2.ver = Y + 1;
	}
	
	tiles.vector[1] = tile1;
	tiles.vector[2] = tile2;
	return tiles;
}


int move_is_valid(GridCoords cur_pos) {
	int i;
	for (i = 0; i < possible_moves.size; i++)
		if (possible_moves.vector[i].orr == cur_pos.orr && possible_moves.vector[i].ver == cur_pos.ver)
			return 1;
		
	return 0;
}


// 1 for p1, 2 for p2, 0 for false
int is_winning_position(GridCoords pos, uint8_t player) {
	if (player == 1) {
		
		if (game_status.turn == 1)
			if (pos.ver == 0)
				return 1;
		
		if (game_status.turn == 2)
			if (pos.ver == 6)
				return 1;
	}
		
	if (player == 2) {
		
		if (game_status.turn == 1)
			if (pos.ver == 6)
				return 1;
		
		if (game_status.turn == 2)
			if (pos.ver == 0)
				return 1;
	}
		
	return 0;
}

// 1 for p1, 2 for p2, 0 for false
int is_win() {
	if (game_status.turn == 1)
		if (game_status.pos_p1.ver == 0)
			return 1;
		
	if (game_status.turn == 2)
		if (game_status.pos_p2.ver == 6)
			return 2;
		
	return 0;
}


void register_player_move(void) {
	uint8_t turn = game_status.turn, X, Y;
	uint32_t move;
	
	if (turn == 1) {
		X = game_status.pos_p1.orr;
		Y = game_status.pos_p1.ver;
	} else if (turn == 2) {
		X = game_status.pos_p2.orr;
		Y = game_status.pos_p2.ver;
	}
	
	move = turn - 1; // first 8 bits: PlayerID
	move = move << 4; // second 4 bits: Player/Wall move: 		0 <= player move
	move = move << 4;	// third 4 bits: Wall Orientation: 			0 <= not in wall mode
	move = (move << 8) | Y;
	move = (move << 8) | X;
	
	game_status.move = move;
	disable_timeout_registration = 1;
}

void register_wall_move(uint8_t X, uint8_t Y, uint8_t orientation) {
	uint8_t turn = game_status.turn;
	uint32_t move;
	
	move = turn - 1; // first 8 bits: PlayerID
	move = (move << 4) | 1; // second 4 bits: Player/Wall move: 						1 <= wall move
	move = (move << 4) | orientation;	// third 4 bits: Wall Orientation:		#define Horizontal 1; #define Vertical 0
	move = (move << 8) | Y;
	move = (move << 8) | X;
	
	game_status.move = move;
	disable_timeout_registration = 1;
}

void register_timeout_move(void) {
	uint8_t turn = game_status.turn, X, Y;
	uint32_t move;
	
	if (disable_timeout_registration == 1)
		return;
	
	if (turn == 1) {
		X = game_status.pos_p1.orr;
		Y = game_status.pos_p1.ver;
	} else if (turn == 2) {
		X = game_status.pos_p2.orr;
		Y = game_status.pos_p2.ver;
	}
	
	move = turn - 1; // first 8 bits: PlayerID
	move = move << 4; // second 4 bits: Player/Wall move: 		0 <= player move
	move = (move << 4) | 1;	// third 4 bits: Wall Orientation: 			1 <= not in wall mode, but move invalid
	move = (move << 8) | Y;
	move = (move << 8) | X;
	
	game_status.move = move;
}

void reset_disable_timeout_registration(void) {
	disable_timeout_registration = 0;
}
