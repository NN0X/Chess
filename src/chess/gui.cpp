#include "gamestate.h"
#include "../datatypes.h"

void colorPossibleMoves(Board& board, const BoardState& boardState, int from)
{
        if (boardState.pieces[from].type == NONE)
                return;
        if (boardState.pieces[from].isWhite != boardState.isWhiteTurn)
                return;

        Color highlightColor = {0.8f, 0.2f, 0.2f};

        Color colorMixedWhite = {
                (board.whiteColor.r + highlightColor.r) / 2,
                (board.whiteColor.g + highlightColor.g) / 2,
                (board.whiteColor.b + highlightColor.b) / 2
        };

        Color colorMixedBlack = {
                (board.blackColor.r + highlightColor.r) / 2,
                (board.blackColor.g + highlightColor.g) / 2,
                (board.blackColor.b + highlightColor.b) / 2
        };

        for (int i = 0; i < 64; i++)
        {
                if (boardState.canMoveTo[from][i])
                {
                        if ((i / 8 + i % 8) % 2 == 0)
                                board.squares[i].color = colorMixedWhite;
                        else
                                board.squares[i].color = colorMixedBlack;
                }
                else
                {
                        if ((i / 8 + i % 8) % 2 == 0)
                                board.squares[i].color = board.whiteColor;
                        else
                                board.squares[i].color = board.blackColor;
                }
        }
}

void recolorBoard(Board& board)
{
        for (int i = 0; i < 64; i++)
        {
                if ((i / 8 + i % 8) % 2 == 0)
                        board.squares[i].color = board.whiteColor;
                else
                        board.squares[i].color = board.blackColor;
        }
}

int getSquareIndexAtPostition(float x, float y, GLFWwindow* window, const Board& board)
{
        for (int i = 0; i < 64; i++)
        {
                Square square = board.squares[i];
                if (square.in(x, y))
                        return i;
        }
        return -1;
}
