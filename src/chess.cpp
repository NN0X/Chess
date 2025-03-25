#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <iostream>
#include <array>
#include <cstdint>

#include "gui.h"
#include "datatypes.h"

struct Board
{
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
        bool hasMoved;
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
};

void printBoardState(const BoardState& boardState)
{
        for (int i = 0; i < 64; i++)
        {
                if (i % 8 == 0)
                        std::cout << "\n";
                if (boardState.pieces[i].type == NONE)
                        std::cout << " ";
                else if (boardState.pieces[i].type == PAWN)
                        std::cout << "P";
                else if (boardState.pieces[i].type == KNIGHT)
                        std::cout << "N";
                else if (boardState.pieces[i].type == BISHOP)
                        std::cout << "B";
                else if (boardState.pieces[i].type == ROOK)
                        std::cout << "R";
                else if (boardState.pieces[i].type == QUEEN)
                        std::cout << "Q";
                else if (boardState.pieces[i].type == KING)
                        std::cout << "K";
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

        for (int i = 0; i < 8; i++)
        {
                for (int j = 0; j < 8; j++)
                {
                        int index = i * 8 + j;
                        // pos is the top left corner of the square
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

const std::string pieceTextures[12] = {
        "resources/pawn_w.bmp",
        "resources/knight_w.bmp",
        "resources/bishop_w.bmp",
        "resources/rook_w.bmp",
        "resources/queen_w.bmp",
        "resources/king_w.bmp",
        "resources/pawn_b.bmp",
        "resources/knight_b.bmp",
        "resources/bishop_b.bmp",
        "resources/rook_b.bmp",
        "resources/queen_b.bmp",
        "resources/king_b.bmp"
};

void drawBoard(const Board& board, const BoardState& boardState)
{
        for (int i = 0; i < 64; i++)
        {
                drawSquare(board.squares[i], 0);
                if (boardState.pieces[i].type != NONE)
                        drawTexture(board.squares[i], 1, pieceTextures[boardState.pieces[i].type - 1 + 6 * boardState.pieces[i].isWhite]);
        }
}

const std::string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

int applyFEN(const std::string& fen, BoardState& boardState)
{
        int numKings = 0;

        // piece placement
        int boardIndex = 0;
        int fenIndex = 0;
        for (const char& c : fen)
        {
                if (boardIndex >= 64)
                        return -1;
                if (fenIndex >= fen.size())
                        return -1;
                if (c == ' ')
                        break;
                else if (c == '/')
                        continue;
                else if (c >= '1' && c <= '8')
                {
                        boardIndex += c - '0';
                        continue;
                }
                else if (c == 'R')
                        boardState.pieces[boardIndex] = {ROOK, false, true};
                else if (c == 'r')
                        boardState.pieces[boardIndex] = {ROOK, false, false};
                else if (c == 'N')
                        boardState.pieces[boardIndex] = {KNIGHT, false, true};
                else if (c == 'n')
                        boardState.pieces[boardIndex] = {KNIGHT, false, false};
                else if (c == 'B')
                        boardState.pieces[boardIndex] = {BISHOP, false, true};
                else if (c == 'b')
                        boardState.pieces[boardIndex] = {BISHOP, false, false};
                else if (c == 'Q')
                        boardState.pieces[boardIndex] = {QUEEN, false, true};
                else if (c == 'q')
                        boardState.pieces[boardIndex] = {QUEEN, false, false};
                else if (c == 'K')
                {
                        boardState.pieces[boardIndex] = {KING, false, true};
                        numKings++;
                }
                else if (c == 'k')
                {
                        boardState.pieces[boardIndex] = {KING, false, false};
                        numKings++;
                }
                else if (c == 'P')
                        boardState.pieces[boardIndex] = {PAWN, false, true};
                else if (c == 'p')
                        boardState.pieces[boardIndex] = {PAWN, false, false};
                boardIndex++;
                fenIndex++;
        }

        if (numKings != 2)
                return -1;

        if (fenIndex >= fen.size())
        {
                boardState.isWhiteTurn = true;
                boardState.whiteCanCastleKingside = true;
                boardState.whiteCanCastleQueenside = true;
                boardState.blackCanCastleKingside = true;
                boardState.blackCanCastleQueenside = true;
                boardState.enPassantSquare = -1;
                boardState.halfMoveClock = 0;
                boardState.fullMoveClock = 1;
                return 0;
        }

        // active color
        if (fen[fenIndex] == 'w')
                boardState.isWhiteTurn = true;
        else if (fen[fenIndex] == 'b')
                boardState.isWhiteTurn = false;
        else
                return -1;

        fenIndex += 2;

        // castling availability
        std::string castlingAvailability = "";
        while (fen[fenIndex] != ' ')
        {
                castlingAvailability += fen[fenIndex];
                fenIndex++;
        }
        if (castlingAvailability.find('K') != std::string::npos)
                boardState.whiteCanCastleKingside = true;
        else
                boardState.whiteCanCastleKingside = false;
        if (castlingAvailability.find('Q') != std::string::npos)
                boardState.whiteCanCastleQueenside = true;
        else
                boardState.whiteCanCastleQueenside = false;
        if (castlingAvailability.find('k') != std::string::npos)
                boardState.blackCanCastleKingside = true;
        else
                boardState.blackCanCastleKingside = false;
        if (castlingAvailability.find('q') != std::string::npos)
                boardState.blackCanCastleQueenside = true;
        else
                boardState.blackCanCastleQueenside = false;

        fenIndex++;

        // en passant target square
        if (fen[fenIndex] == '-')
                boardState.enPassantSquare = -1;
        else
        {
                int file = fen[fenIndex] - 'a';
                int rank = fen[fenIndex + 1] - '1';
                boardState.enPassantSquare = rank * 8 + file;
        }

        fenIndex += 2;

        // halfmove and fullmove clock
        std::string halfMoveClock = "";
        while (fen[fenIndex] != ' ')
        {
                halfMoveClock += fen[fenIndex];
                fenIndex++;
        }

        std::string fullMoveClock = "";
        while (fen[fenIndex] != ' ')
        {
                fullMoveClock += fen[fenIndex];
                fenIndex++;
        }

        try
        {
                boardState.halfMoveClock = std::stoi(halfMoveClock);
                boardState.fullMoveClock = std::stoi(fullMoveClock);
        }
        catch (const std::invalid_argument& e)
        {
                return -1;
        }

        return 0;
}

int awake(GLFWwindow** window)
{
        *window = init("Chess", 800, 800);
        if (window == NULL)
                return -1;

        return 0;
}

int update(GLFWwindow* window)
{
        Board board = generateBoard(800, 600, false, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f});
        BoardState boardState;
        applyFEN(startFEN, boardState);
        printBoardState(boardState);
        while (!glfwWindowShouldClose(window))
        {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                reshapeWindow(window);
                drawBoard(board, boardState);
                glfwSwapBuffers(window);

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
