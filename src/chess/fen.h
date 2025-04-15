#ifndef CHESS_FEN_H
#define CHESS_FEN_H

#include "gamestate.h"

int applyFEN(const std::string& fen, BoardState& boardState);
int getFEN(std::string& fen, const BoardState& boardState);

#endif // CHESS_FEN_H
