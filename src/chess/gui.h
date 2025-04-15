#ifndef CHESS_GUI_H
#define CHESS_GUI_H

#include <GLFW/glfw3.h>

#include "gamestate.h"

void colorPossibleMoves(Board& board, const BoardState& boardState, int from);
void recolorBoard(Board& board);
int getSquareIndexAtPostition(float x, float y, GLFWwindow* window, const Board& board);

#endif // CHESS_GUI_H
