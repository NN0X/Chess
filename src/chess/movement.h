#ifndef CHESS_MOVEMENT_H
#define CHESS_MOVEMENT_H

#include "gamestate.h"

void generatePossibleMoves(BoardState& boardState);
void eliminateCheckMoves(BoardState& boardState, int i);
bool checkLegality(const BoardState& boardState, int fromIndex, int toIndex);
void movePiece(BoardState& boardState, const Board& board, int fromIndex, int toIndex);

#endif //CHESS_MOVEMENT_H
