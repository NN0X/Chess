#ifndef CHESS_GAMESTATE_H
#define CHESS_GAMESTATE_H

#include <GLFW/glfw3.h>

#include <array>
#include <cstdint>

#include "../datatypes.h"

struct Board
{
        Color whiteColor;
        Color blackColor;
        std::array<Square, 64> squares; // 0 is a8, 63 is h1
};

enum PieceType
{
        NONE = 0,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
};

struct Piece
{
        uint8_t type;
        bool isWhite;
};

struct BoardState
{
        bool isWhiteTurn;
        bool whiteCanCastleKingside;
        bool whiteCanCastleQueenside;
        bool blackCanCastleKingside;
        bool blackCanCastleQueenside;

        int enPassantSquare; // -1 if no en passant square

        int halfMoveClock;
        int fullMoveClock;

        std::array<Piece, 64> pieces; // 0 is a8, 63 is h1
        std::array<std::array<bool, 64>, 64> canMoveTo; // array of possible moves for each piece

        std::string moveHistory; // TODO: implement this
        std::vector<std::string> moveHistoryVector; // TODO: implement this to allow undoing moves and checking for 3fold repetition
};

enum GameState
{
        CONTINUE = 0,
        CHECKMATE = 1,
        STALEMATE = 2
};

void printBoardState(const BoardState& boardState);
void printAvailableMoves(const BoardState& boardState, int from);
Board generateBoard(int width, int height, bool isBlackPersp, Color whiteColor, Color blackColor);
void drawBoard(const Board& board, const BoardState& boardState);
int checkGameState(const BoardState& boardState);
void processInput(GLFWwindow* window, BoardState& boardState, Board& board, double& prevXpos, double& prevYpos, bool& isMovingPiece);

#endif // CHESS_GAMESTATE_H
