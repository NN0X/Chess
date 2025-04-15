#include <iostream>

#include "gamestate.h"
#include "../gui.h"
#include "gui.h"
#include "movement.h"

void printBoardState(const BoardState& boardState)
{
        for (int i = 0; i < 64; i++)
        {
                if (i % 8 == 0)
                        std::cout << "\n";
                if (boardState.pieces[i].type == NONE)
                        std::cout << " ";
                else if (boardState.pieces[i].type == PAWN && boardState.pieces[i].isWhite)
                        std::cout << "P";
                else if (boardState.pieces[i].type == KNIGHT && boardState.pieces[i].isWhite)
                        std::cout << "N";
                else if (boardState.pieces[i].type == BISHOP && boardState.pieces[i].isWhite)
                        std::cout << "B";
                else if (boardState.pieces[i].type == ROOK && boardState.pieces[i].isWhite)
                        std::cout << "R";
                else if (boardState.pieces[i].type == QUEEN && boardState.pieces[i].isWhite)
                        std::cout << "Q";
                else if (boardState.pieces[i].type == KING && boardState.pieces[i].isWhite)
                        std::cout << "K";
                else if (boardState.pieces[i].type == PAWN)
                        std::cout << "p";
                else if (boardState.pieces[i].type == KNIGHT)
                        std::cout << "n";
                else if (boardState.pieces[i].type == BISHOP)
                        std::cout << "b";
                else if (boardState.pieces[i].type == ROOK)
                        std::cout << "r";
                else if (boardState.pieces[i].type == QUEEN)
                        std::cout << "q";
                else if (boardState.pieces[i].type == KING)
                        std::cout << "k";
                else
                        std::cout << "?";
        }
        std::cout << "\n";
        std::cout << "Turn: " << (boardState.isWhiteTurn ? "White" : "Black") << "\n";

        std::cout << "Castling: ";
        if (boardState.whiteCanCastleKingside)
                std::cout << "K";
        if (boardState.whiteCanCastleQueenside)
                std::cout << "Q";
        if (boardState.blackCanCastleKingside)
                std::cout << "k";
        if (boardState.blackCanCastleQueenside)
                std::cout << "q";
        std::cout << "\n";
}

void printAvailableMoves(const BoardState& boardState, int from)
{
        std::cout << "Available moves for piece at " << from << ": \n";
        for (int i = 0; i < 64; i++)
        {
                std::cout << (boardState.canMoveTo[from][i] ? i : -1) << " ";
                if (i % 8 == 7)
                        std::cout << "\n";
        }
        std::cout << "\n";
}

Board generateBoard(int width, int height, bool isBlackPersp, Color whiteColor, Color blackColor)
{
        int smaller = width < height ? width : height;
        float sqrWidth = (float)smaller / 8;
        float sqrWidthScaledToOpenGL = sqrWidth / (width / 2.0);
        sqrWidth = sqrWidthScaledToOpenGL;

        Board board;

        board.whiteColor = whiteColor;
        board.blackColor = blackColor;

        for (int i = 0; i < 8; i++)
        {
                for (int j = 0; j < 8; j++)
                {
                        int index = i * 8 + j;
                        board.squares[index].pos.x = -1.0f + j * sqrWidth;
                        board.squares[index].pos.y = 1.0f - i * sqrWidth;
                        board.squares[index].size = sqrWidth;
                        if ((i + j) % 2 == isBlackPersp)
                                board.squares[index].color = whiteColor;
                        else
                                board.squares[index].color = blackColor;
                }
        }

        return board;
}

const std::string pieceTextures[13] = {
        "dummy",
        "resources/white-pawn.png",
        "resources/white-knight.png",
        "resources/white-bishop.png",
        "resources/white-rook.png",
        "resources/white-queen.png",
        "resources/white-king.png",
        "resources/black-pawn.png",
        "resources/black-knight.png",
        "resources/black-bishop.png",
        "resources/black-rook.png",
        "resources/black-queen.png",
        "resources/black-king.png"
};

void drawBoard(const Board& board, const BoardState& boardState)
{
        for (int i = 0; i < 64; i++)
        {
                drawSquare(board.squares[i], 0);
                int type = boardState.pieces[i].type;
                if (type != NONE)
                {
                        drawTexture(board.squares[i], 1, pieceTextures[type + (boardState.pieces[i].isWhite ? 0 : 6)]);
                }
        }
}

int checkGameState(const BoardState& boardState)
{
        bool availableMoves = false;
        for (int i = 0; i < 64; i++)
        {
                if (boardState.pieces[i].isWhite == boardState.isWhiteTurn)
                {
                        for (int j = 0; j < 64; j++)
                        {
                                if (boardState.canMoveTo[i][j])
                                {
                                        availableMoves = true;
                                        break;
                                }
                        }
                }
                if (availableMoves)
                        break;
        }

        bool kingInCheck = false;
        for (int i = 0; i < 64; i++)
        {
                if (boardState.pieces[i].type == KING && boardState.pieces[i].isWhite == boardState.isWhiteTurn)
                {
                        for (int j = 0; j < 64; j++)
                        {
                                if (boardState.canMoveTo[j][i] && boardState.pieces[j].isWhite != boardState.isWhiteTurn)
                                {
                                        kingInCheck = true;
                                        break;
                                }
                        }
                }
                if (kingInCheck)
                        break;
        }

        if (kingInCheck && !availableMoves)
        {
                return CHECKMATE;
        }
        else if (!kingInCheck && !availableMoves)
        {
                return STALEMATE;
        }

        return CONTINUE;
}

void processInput(GLFWwindow* window, BoardState& boardState, Board& board, double& prevXpos, double& prevYpos, bool& isMovingPiece)
{
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !isMovingPiece)
        {
                glfwGetCursorPos(window, &prevXpos, &prevYpos);
                convertToOpenGLCoords(prevXpos, prevYpos, window);
                int from = getSquareIndexAtPostition(prevXpos, prevYpos, window, board);
                generatePossibleMoves(boardState);
                eliminateCheckMoves(boardState, from);
                colorPossibleMoves(board, boardState, from);
                //printAvailableMoves(boardState, from);
                isMovingPiece = true;
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && isMovingPiece)
        {
                double xpos, ypos;
                glfwGetCursorPos(window, &xpos, &ypos);
                convertToOpenGLCoords(xpos, ypos, window);
                int from = getSquareIndexAtPostition(prevXpos, prevYpos, window, board);
                int to = getSquareIndexAtPostition(xpos, ypos, window, board);
                movePiece(boardState, board, from, to);
                recolorBoard(board);
                isMovingPiece = false;
        }
}
