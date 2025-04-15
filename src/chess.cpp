#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <cstdint>

#include "gui.h"
#include "datatypes.h"
#include "chess/gamestate.h"
#include "chess/movement.h"
#include "chess/fen.h"

int awake(GLFWwindow** window)
{
        *window = init("Chess", 800, 800);
        if (window == NULL)
                return -1;

        return 0;
}

const std::string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const std::string castleTestFEN = "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1";

int update(GLFWwindow* window)
{
        Board board = generateBoard(800, 800, false, {1.f, 1.f, 1.f}, {0.34f, 0.2f, 0.2f});
        BoardState boardState;
        boardState.pieces.fill({NONE, false});
        if (applyFEN(startFEN, boardState) != 0)
        {
                std::cerr << "Error parsing FEN\n";
                return -1;
        }
        //printBoardState(boardState);
        double xpos, ypos;
        bool isMovingPiece = false;
        while (!glfwWindowShouldClose(window))
        {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                reshapeWindow(window);
                drawBoard(board, boardState);
                glfwSwapBuffers(window);

                processInput(window, boardState, board, xpos, ypos, isMovingPiece);

                BoardState tempBoardState = boardState;
                generatePossibleMoves(tempBoardState);
                for (int i = 0; i < 64; i++)
                {
                        eliminateCheckMoves(tempBoardState, i);
                }
                switch (checkGameState(tempBoardState))
                {
                        case CHECKMATE:
                                std::cout << "Checkmate!\n";
                                std::cout << "Winner: " << (boardState.isWhiteTurn ? "Black" : "White") << "\n";
                                glfwSetWindowShouldClose(window, true);
                                break;
                        case STALEMATE:
                                std::cout << "Stalemate!\n";
                                glfwSetWindowShouldClose(window, true);
                                break;
                        default:
                                break;
                }
                glfwPollEvents();
        }

        return 0;
}

int shutdown()
{
        glfwTerminate();

        return 0;
}

int chess()
{
        GLFWwindow* window;
        awake(&window);
        update(window);
        shutdown();

        return 0;
}
