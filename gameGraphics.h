void gameCoord_DrawBox(uint16_t X, uint16_t Y, uint16_t color);
void gameCoord_DrawWall(uint16_t X, uint16_t Y, int horizontal);
void gameCoord_DrawGrid(uint16_t sizeX, uint16_t sizeY);

void gameInd_DrawBox(uint16_t indexOrr, uint16_t indexVert, uint16_t color);
void gameInd_DrawWall(uint16_t indexOrr, uint16_t indexVert, int horizontal);
void gameInd_DrawWall_choosecolor(uint16_t indexOrr, uint16_t indexVert, int orientation, uint16_t color);
void gameInd_DrawTempWall(uint16_t indexOrr, uint16_t indexVert, int orientation);
void gameInd_DrawGrid(uint16_t sizeX, uint16_t sizeY);

void gameInd_IlluminateBox(uint16_t indexOrr, uint16_t indexVert);
void gameInd_deIlluminateBox(uint16_t indexOrr, uint16_t indexVert);

void gameCoord_DrawPG(uint8_t X, uint8_t Y, uint16_t color);
void gameInd_DrawPG(uint8_t indexOrr, uint8_t indexVert, uint16_t color);
void gameInd_DelPG(uint8_t indexOrr, uint8_t indexVert);
void del_phantoms(Vector possible_moves);

void gameCoord_DrawBoxBigger(uint16_t X, uint16_t Y, uint16_t color);
void game_CleanTime(void);
void draw_wall_in_centre(void);
void redraw_walls(Status game_status);

void show_winner_msg(int player);
void show_walls_msg(void);
void clean_walls_msg(void);

void redraw_all(Status game_status);
