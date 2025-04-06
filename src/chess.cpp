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
};

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

const std::string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

/*
rnbqkbnr
pppppppp




PPPPPPPP
RNBQKBNR


-1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1
-1 -1 -1 -1 -1 -1 -1 -1

*/

void generatePawnMoves(BoardState& boardState, int i)
{
        int direction = boardState.pieces[i].isWhite ? -1 : 1;
        int startRank = boardState.pieces[i].isWhite ? 6 : 1;
        int rank = i / 8;
        int file = i % 8;
        int forwardSquare = i + direction * 8;
        int forwardTwoSquare = i + direction * 16;
        int leftCaptureSquare = i + direction * 8 - 1;
        int rightCaptureSquare = i + direction * 8 + 1;

        bool canMoveForward = (rank + direction >= 0 && rank + direction < 8) && boardState.pieces[forwardSquare].type == NONE;
        bool canMoveForwardTwo = (rank + direction * 2 >= 0 && rank + direction * 2 < 8) && boardState.pieces[forwardTwoSquare].type == NONE;
        bool canCaptureLeft = (rank + direction >= 0 && rank + direction < 8) && (file - 1 >= 0) && boardState.pieces[leftCaptureSquare].type != NONE && boardState.pieces[leftCaptureSquare].isWhite != boardState.pieces[i].isWhite;
        bool canCaptureRight = (rank + direction >= 0 && rank + direction < 8) && (file + 1 < 8) && boardState.pieces[rightCaptureSquare].type != NONE && boardState.pieces[rightCaptureSquare].isWhite != boardState.pieces[i].isWhite;

        if (canMoveForward)
        {
                boardState.canMoveTo[i][forwardSquare] = true;
                if (canMoveForwardTwo && rank == startRank)
                {
                        boardState.canMoveTo[i][forwardTwoSquare] = true;
                }
        }

        if (canCaptureLeft)
        {
                boardState.canMoveTo[i][leftCaptureSquare] = true;
        }

        if (canCaptureRight)
        {
                boardState.canMoveTo[i][rightCaptureSquare] = true;
        }

        // En passant

        int enPassantRank = boardState.enPassantSquare / 8;
        const int whiteEnPassantRank = 2;
        const int blackEnPassantRank = 7;

        bool enPassantCorrectColor = boardState.pieces[i].isWhite && enPassantRank == whiteEnPassantRank ||
                                     !boardState.pieces[i].isWhite && enPassantRank == blackEnPassantRank;

        if (boardState.enPassantSquare != -1 && enPassantCorrectColor)
        {
                int enPassantFile = boardState.enPassantSquare % 8;
                bool enPassantSquareFree = boardState.pieces[boardState.enPassantSquare].type == NONE;
                if (enPassantRank == rank + direction && (enPassantFile == file - 1 || enPassantFile == file + 1) && enPassantSquareFree)
                {
                        boardState.canMoveTo[i][boardState.enPassantSquare] = true;
                }
        }
}

void generateKnightMoves(BoardState& boardState, int i)
{
        const int knightMoves[8][2] = {
                {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
        };

        int rank = i / 8;
        int file = i % 8;

        for (int j = 0; j < 8; j++)
        {
                int newRank = rank + knightMoves[j][0];
                int newFile = file + knightMoves[j][1];
                if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8)
                {
                        int newIndex = newRank * 8 + newFile;
                        if (boardState.pieces[newIndex].type == NONE || boardState.pieces[newIndex].isWhite != boardState.pieces[i].isWhite)
                        {
                                boardState.canMoveTo[i][newIndex] = true;
                        }
                }
        }
}

void generateBishopMoves(BoardState& boardState, int i)
{
        int rank = i / 8;
        int file = i % 8;

        for (int r = -1; r <= 1; r += 2)
        {
                for (int f = -1; f <= 1; f += 2)
                {
                        for (int j = 1; j < 8; j++)
                        {
                                int newRank = rank + r * j;
                                int newFile = file + f * j;
                                if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8)
                                {
                                        int newIndex = newRank * 8 + newFile;
                                        if (boardState.pieces[newIndex].type == NONE)
                                        {
                                                boardState.canMoveTo[i][newIndex] = true;
                                        }
                                        else if (boardState.pieces[newIndex].isWhite != boardState.pieces[i].isWhite)
                                        {
                                                boardState.canMoveTo[i][newIndex] = true;
                                                break;
                                        }
                                        else
                                        {
                                                break;
                                        }
                                }
                        }
                }
        }
}

void generateRookMoves(BoardState& boardState, int i)
{
        int rank = i / 8;
        int file = i % 8;

        for (int r = -1; r <= 1; r += 2)
        {
                for (int j = 1; j < 8; j++)
                {
                        int newRank = rank + r * j;
                        if (newRank >= 0 && newRank < 8)
                        {
                                int newIndex = newRank * 8 + file;
                                if (boardState.pieces[newIndex].type == NONE)
                                {
                                        boardState.canMoveTo[i][newIndex] = true;
                                }
                                else if (boardState.pieces[newIndex].isWhite != boardState.pieces[i].isWhite)
                                {
                                        boardState.canMoveTo[i][newIndex] = true;
                                        break;
                                }
                                else
                                {
                                        break;
                                }
                        }
                }
        }

        for (int f = -1; f <= 1; f += 2)
        {
                for (int j = 1; j < 8; j++)
                {
                        int newFile = file + f * j;
                        if (newFile >= 0 && newFile < 8)
                        {
                                int newIndex = rank * 8 + newFile;
                                if (boardState.pieces[newIndex].type == NONE)
                                {
                                        boardState.canMoveTo[i][newIndex] = true;
                                }
                                else if (boardState.pieces[newIndex].isWhite != boardState.pieces[i].isWhite)
                                {
                                        boardState.canMoveTo[i][newIndex] = true;
                                        break;
                                }
                                else
                                {
                                        break;
                                }
                        }
                }
        }
}

void generateKingMoves(BoardState& boardState, int i)
{
        const int kingMoves[8][2] = {
                {1, 0}, {1, 1}, {0, 1}, {-1, 1},
                {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
        };

        int rank = i / 8;
        int file = i % 8;

        for (int j = 0; j < 8; j++)
        {
                int newRank = rank + kingMoves[j][0];
                int newFile = file + kingMoves[j][1];
                if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8)
                {
                        int newIndex = newRank * 8 + newFile;
                        if (boardState.pieces[newIndex].type == NONE || boardState.pieces[newIndex].isWhite != boardState.pieces[i].isWhite)
                        {
                                boardState.canMoveTo[i][newIndex] = true;
                        }
                }
        }

        // Castling
        if (boardState.whiteCanCastleKingside && boardState.pieces[5].type == NONE && boardState.pieces[6].type == NONE)
        {
                boardState.canMoveTo[i][6] = true;
        }
        if (boardState.whiteCanCastleQueenside && boardState.pieces[3].type == NONE && boardState.pieces[2].type == NONE)
        {
                boardState.canMoveTo[i][2] = true;
        }
        if (boardState.blackCanCastleKingside && boardState.pieces[61].type == NONE && boardState.pieces[62].type == NONE)
        {
                boardState.canMoveTo[i][62] = true;
        }
        if (boardState.blackCanCastleQueenside && boardState.pieces[59].type == NONE && boardState.pieces[58].type == NONE)
        {
                boardState.canMoveTo[i][58] = true;
        }
}

void eliminateCheckMoves(BoardState& boardState, int i)
{
        for (int j = 0; j < 64; j++)
        {
                if (boardState.canMoveTo[i][j])
                {
                        // Check if the move puts the king in check
                        // If it does, eliminate the move

                        // if king is in check then
                        // check if the move prevents the king from being in check
                        // if it does not then eliminate the move
                }
        }
}

void generatePossibleMoves(BoardState& boardState)
{
        boardState.canMoveTo.fill({false}); // Clear the canMoveTo array
        for (int i = 0; i < 64; i++)
        {
                const Piece piece = boardState.pieces[i];
                switch (piece.type)
                {
                        case NONE:
                                break;
                        case PAWN:
                                generatePawnMoves(boardState, i);
                                break;
                        case KNIGHT:
                                generateKnightMoves(boardState, i);
                                break;
                        case BISHOP:
                                generateBishopMoves(boardState, i);
                                break;
                        case ROOK:
                                generateRookMoves(boardState, i);
                                break;
                        case QUEEN:
                                generateBishopMoves(boardState, i);
                                generateRookMoves(boardState, i);
                                break;
                        case KING:
                                generateKingMoves(boardState, i);
                                break;
                        default:
                                std::cerr << "Invalid piece type\n";
                                break;
                }
                eliminateCheckMoves(boardState, i);
        }
}

bool isGameOver(const BoardState& boardState)
{
        // Check if the game is over
        // Check for checkmate or stalemate
        // If the game is over, return true
        // Otherwise, return false

        return false;
}

int applyFEN(const std::string& fen, BoardState& boardState)
{
        int numKings = 0;
        boardState.pieces.fill({NONE, false}); // Clear the board

        // Piece placement parsing
        size_t fenIndex = 0;
        int boardIndex = 0;

        while (fenIndex < fen.size() && fen[fenIndex] != ' ')
        {
                char c = fen[fenIndex++];
                if (c == '/')
                {
                        continue;
                }
                else if (isdigit(c))
                {
                        int emptySquares = c - '0';
                        boardIndex += emptySquares;
                        if (boardIndex > 64)
                                return -1; // Too many squares
                }
                else
                {
                        if (boardIndex >= 64)
                                return -1; // Exceeded board bounds

                        Piece piece = {NONE, false};
                        switch (toupper(c))
                        {
                                case 'R': piece.type = ROOK; break;
                                case 'N': piece.type = KNIGHT; break;
                                case 'B': piece.type = BISHOP; break;
                                case 'Q': piece.type = QUEEN; break;
                                case 'K': piece.type = KING; numKings++; break;
                                case 'P': piece.type = PAWN; break;
                                default: return -1; // Invalid character
                        }
                        piece.isWhite = isupper(c);
                        boardState.pieces[boardIndex++] = piece;
                }
        }

        if (boardIndex != 64 || numKings != 2)
                return -1;

        if (fenIndex > fen.size() - 1)
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

        fenIndex++; // Skip space

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

        fenIndex++; // Skip space
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

        generatePossibleMoves(boardState);

        return 0;
}

int awake(GLFWwindow** window)
{
        *window = init("Chess", 800, 800);
        if (window == NULL)
                return -1;

        return 0;
}

bool checkLegality(const BoardState& boardState, int fromIndex, int toIndex)
{
        // check if on board
        if (fromIndex < 0 || fromIndex > 63 || toIndex < 0 || toIndex > 63)
        {
                std::cout << "From or to index is out of bounds\n";
                return false;
        }
        // check if same square
        if (fromIndex == toIndex)
        {
                std::cout << "From and to index are the same\n";
                return false;
        }

        // check if correct color
        if (boardState.pieces[fromIndex].type == NONE)
        {
                std::cout << "No piece at from index\n";
                return false;
        }

        if (boardState.pieces[fromIndex].isWhite != boardState.isWhiteTurn)
        {
                std::cout << "From index is occupied by opposite color\n";
                return false;
        }

        if (boardState.pieces[toIndex].isWhite == boardState.isWhiteTurn && boardState.pieces[toIndex].type != NONE)
        {
                std::cout << "To index is occupied by same color\n";
                return false;
        }

        if (!boardState.canMoveTo[fromIndex][toIndex])
        {
                std::cout << "Move is not legal\n";
                return false;
        }

        std::cout << "Move is legal\n";
        return true;
}

void pawnMoved(BoardState& boardState, int fromIndex, int toIndex)
{
        if (boardState.pieces[toIndex].type == PAWN)
        {
                // check if en passant capture
                int fromFile = fromIndex % 8;
                int toFile = toIndex % 8;
                int fileDistance = toFile - fromFile;
                if (boardState.enPassantSquare == toIndex && (fileDistance == 1 || fileDistance == -1))
                {
                        int capturedPawnIndex = toIndex + (boardState.pieces[toIndex].isWhite ? 8 : -8);
                        boardState.pieces[capturedPawnIndex] = {NONE, false};
                        boardState.enPassantSquare = -1;
                }

                // check for en passant possibility
                int fromRank = fromIndex / 8;
                int toRank = toIndex / 8;
                int rankDistance = toRank - fromRank;
                if (rankDistance == 2 || rankDistance == -2)
                {
                        boardState.enPassantSquare = fromIndex + (boardState.pieces[fromIndex].isWhite ? -8 : 8);
                }
                else
                {
                        boardState.enPassantSquare = -1;
                }

                // check for promotion
                if (toRank == 0 && boardState.pieces[toIndex].isWhite)
                {
                        boardState.pieces[toIndex] = {QUEEN, true};
                }
                else if (toRank == 7 && !boardState.pieces[toIndex].isWhite)
                {
                        boardState.pieces[toIndex] = {QUEEN, false};
                }
        }
        else
        {
                boardState.enPassantSquare = -1;
        }
}

void rookMoved(BoardState& boardState, int fromIndex, int toIndex)
{
        if (boardState.pieces[fromIndex].type == ROOK)
        {
                if (fromIndex == 0)
                        boardState.whiteCanCastleQueenside = false;
                else if (fromIndex == 7)
                        boardState.whiteCanCastleKingside = false;
                else if (fromIndex == 56)
                        boardState.blackCanCastleQueenside = false;
                else if (fromIndex == 63)
                        boardState.blackCanCastleKingside = false;
        }
}

void kingMoved(BoardState& boardState, int fromIndex, int toIndex)
{
        if (boardState.pieces[fromIndex].type == KING)
        {
                if (fromIndex == 4)
                {
                        boardState.whiteCanCastleKingside = false;
                        boardState.whiteCanCastleQueenside = false;
                }
                else if (fromIndex == 60)
                {
                        boardState.blackCanCastleKingside = false;
                        boardState.blackCanCastleQueenside = false;
                }
        }
}

void movePiece(BoardState& boardState, const Board& board, int fromIndex, int toIndex)
{
        if (!checkLegality(boardState, fromIndex, toIndex))
                return;

        boardState.pieces[toIndex] = boardState.pieces[fromIndex];
        boardState.pieces[fromIndex] = {NONE, false};
        boardState.isWhiteTurn = !boardState.isWhiteTurn;
        boardState.halfMoveClock++;
        boardState.fullMoveClock += boardState.isWhiteTurn ? 1 : 0;

        pawnMoved(boardState, fromIndex, toIndex);
        rookMoved(boardState, fromIndex, toIndex);
        kingMoved(boardState, fromIndex, toIndex);

        std::cout << "Turn: " << (boardState.isWhiteTurn ? "White" : "Black") << "\n";
        if (boardState.enPassantSquare != -1)
        {
                std::cout << "En passant square: " << boardState.enPassantSquare << "\n";
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

void colorPossibleMoves(Board& board, const BoardState& boardState, int from)
{
        // redish color

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
                colorPossibleMoves(board, boardState, from);
                printAvailableMoves(boardState, from);
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
        printBoardState(boardState);
        double xpos, ypos;
        bool isMovingPiece = false;
        while (!glfwWindowShouldClose(window))
        {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                reshapeWindow(window);
                drawBoard(board, boardState);
                glfwSwapBuffers(window);

                processInput(window, boardState, board, xpos, ypos, isMovingPiece);
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
