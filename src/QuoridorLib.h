#include "Graph/Graph.h"
#include <stdint.h>

#define Horizontal 1
#define Vertical 0
#define TIMEOUT 20

typedef struct GridCoords { 		
	int8_t orr, ver; // from 0,0 to 6,6 included. A 7x7 square grid
} GridCoords;

typedef struct Status {					
	int8_t walls_remaining_p1, walls_remaining_p2; // starting with 8
	int8_t xi_max, yi_max; // 7 x 7 counting from 0 means they're both 6. i stands for index instead of (LCD) coordinates
	int8_t win; // starts with 0, set to 1 if p1 wins, else 2
	int8_t turn; // 1 for player 1, 2 for player 2
	int8_t started;
	GridCoords pos_p1, pos_p2; // both start in the 4th square (orr pos = 3, at opposite ends ofc
	int8_t remaining_time; // starts at 20 each turn, decreased by one each second
	int8_t vertical_walls[6][7];
	int8_t horizontal_walls[7][6];
	int8_t center_of_walls_table[6][6]; // init value is -1. 0 (Vertical), 1 (Horizontal)
	
	uint32_t move; // Move information. Structure conforming to specifiations
} Status;

typedef struct Vector {
	GridCoords vector[4];
	int8_t size;
} Vector;

typedef struct Surrounding_Walls {
	int8_t left;
	int8_t up;
	int8_t bot;
	int8_t right;
} Surrounding_Walls;


// Adiajency List Graph...


void game_setup(void);

void showP1(void);
void showP2(void);
void showPhantom(GridCoords pos);

void updateInfometrics_wallsP1(void);
void updateInfometrics_wallsP2(void);
void updateInfometrics_turnWhoose(void);
void updateInfometrics_time(void);

Vector get_possible_moves(void);
Vector get_possible_moves_bypos(GridCoords specified_position);
GridCoords get_cur_player_pos(void);
GridCoords get_opp_player_pos(void);

void change_turn(void);

Surrounding_Walls get_surrounding_walls(uint8_t X, uint8_t Y);
int* get_wall_blocked_positions(GridCoords tile, int orientation);
int move_is_valid(GridCoords cur_pos);
int try_place_wall(uint8_t X, uint8_t Y, uint8_t orientation);
int try_place_wall_checkopp(uint8_t X, uint8_t Y, uint8_t orientation);

Vector centerWall_to_tilesWall (uint8_t X, uint8_t Y, uint8_t orientation);

int is_win(void);
int is_winning_position(GridCoords pos, uint8_t player);

void register_player_move(void);
void register_wall_move(uint8_t X, uint8_t Y, uint8_t orientation);
void register_timeout_move(void);
void reset_disable_timeout_registration(void);
