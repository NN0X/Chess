#include <iostream>

#include "gamestate.h"

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
}

void generateCastlingMoves(BoardState& boardState)
{
        if (boardState.isWhiteTurn)
        {
                if (boardState.whiteCanCastleKingside)
                {
                        if (boardState.pieces[60].type == KING && boardState.pieces[63].type == ROOK &&
                                boardState.pieces[61].type == NONE && boardState.pieces[62].type == NONE)
                        {
                                boardState.canMoveTo[60][62] = true;
                        }
                }
                if (boardState.whiteCanCastleQueenside)
                {
                        if (boardState.pieces[60].type == KING && boardState.pieces[56].type == ROOK &&
                                boardState.pieces[57].type == NONE && boardState.pieces[58].type == NONE)
                        {
                                boardState.canMoveTo[60][58] = true;
                        }
                }
        }
        else
        {
                if (boardState.blackCanCastleKingside)
                {
                        if (boardState.pieces[4].type == KING && boardState.pieces[7].type == ROOK &&
                                boardState.pieces[5].type == NONE && boardState.pieces[6].type == NONE)
                        {
                                boardState.canMoveTo[4][6] = true;
                        }
                }
                if (boardState.blackCanCastleQueenside)
                {
                        if (boardState.pieces[4].type == KING && boardState.pieces[0].type == ROOK &&
                                boardState.pieces[1].type == NONE && boardState.pieces[2].type == NONE)
                        {
                                boardState.canMoveTo[4][2] = true;
                        }
                }
        }

        if (boardState.canMoveTo[60][62])
        {
                const int squaresToCheck[3] = {60, 61, 62};
                for (int i = 0; i < 3; i++)
                {
                        int square = squaresToCheck[i];
                        for (int j = 0; j < 64; j++)
                        {
                                if (boardState.canMoveTo[j][square] && boardState.pieces[j].isWhite != boardState.isWhiteTurn)
                                {
                                        boardState.canMoveTo[60][62] = false;
                                        break;
                                }
                        }
                }
        }

        if (boardState.canMoveTo[60][58])
        {
                const int squaresToCheck[4] = {60, 57, 58, 59};
                for (int i = 0; i < 4; i++)
                {
                        int square = squaresToCheck[i];
                        for (int j = 0; j < 64; j++)
                        {
                                if (boardState.canMoveTo[j][square] && boardState.pieces[j].isWhite != boardState.isWhiteTurn)
                                {
                                        boardState.canMoveTo[60][58] = false;
                                        break;
                                }
                        }
                }
        }

        if (boardState.canMoveTo[4][6])
        {
                const int squaresToCheck[3] = {4, 5, 6};
                for (int i = 0; i < 3; i++)
                {
                        int square = squaresToCheck[i];
                        for (int j = 0; j < 64; j++)
                        {
                                if (boardState.canMoveTo[j][square] && boardState.pieces[j].isWhite != boardState.isWhiteTurn)
                                {
                                        boardState.canMoveTo[4][6] = false;
                                        break;
                                }
                        }
                }
        }

        if (boardState.canMoveTo[4][2])
        {
                const int squaresToCheck[4] = {4, 3, 2, 1};
                for (int i = 0; i < 4; i++)
                {
                        int square = squaresToCheck[i];
                        for (int j = 0; j < 64; j++)
                        {
                                if (boardState.canMoveTo[j][square] && boardState.pieces[j].isWhite != boardState.isWhiteTurn)
                                {
                                        boardState.canMoveTo[4][2] = false;
                                        break;
                                }
                        }
                }
        }
}

void generatePossibleMoves(BoardState& boardState)
{
        boardState.canMoveTo.fill({false});
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
                                return;
                }
        }

        if(!boardState.whiteCanCastleKingside && !boardState.whiteCanCastleQueenside &&
           !boardState.blackCanCastleKingside && !boardState.blackCanCastleQueenside)
        {
                return;
        }
        else
        {
                generateCastlingMoves(boardState);
        }
}

void eliminateCheckMoves(BoardState& boardState, int i)
{
        int kingIndex = -1;
        for (int j = 0; j < 64; j++)
        {
                if (boardState.pieces[j].type == KING &&
                        boardState.pieces[j].isWhite == boardState.isWhiteTurn)
                {
                        kingIndex = j;
                        break;
                }
        }

        for (int j = 0; j < 64; j++)
        {
                if (boardState.canMoveTo[i][j])
                {
                        BoardState tempBoardState = boardState;
                        tempBoardState.pieces[j] = tempBoardState.pieces[i];
                        tempBoardState.pieces[i] = {NONE, false};

                        int newKingIndex = kingIndex;
                        if (boardState.pieces[i].type == KING)
                        {
                                newKingIndex = j;
                        }

                        generatePossibleMoves(tempBoardState);

                        bool kingInCheck = false;
                        for (int k = 0; k < 64; k++)
                        {
                                if (tempBoardState.canMoveTo[k][newKingIndex] &&
                                        tempBoardState.pieces[k].isWhite != boardState.isWhiteTurn &&
                                        tempBoardState.pieces[k].type != KING)
                                {
                                        kingInCheck = true;
                                        break;
                                }
                        }

                        if (kingInCheck)
                        {
                                boardState.canMoveTo[i][j] = false;
                        }
                }
        }
}

bool checkLegality(const BoardState& boardState, int fromIndex, int toIndex)
{
        if (fromIndex < 0 || fromIndex > 63 || toIndex < 0 || toIndex > 63)
        {
                std::cerr << "From or to index is out of bounds\n";
                return false;
        }

        if (fromIndex == toIndex)
        {
                std::cerr << "From and to index are the same\n";
                return false;
        }

        if (boardState.pieces[fromIndex].type == NONE)
        {
                std::cerr << "No piece at from index\n";
                return false;
        }

        if (boardState.pieces[fromIndex].isWhite != boardState.isWhiteTurn)
        {
                std::cerr << "From index is occupied by opposite color\n";
                return false;
        }

        if (boardState.pieces[toIndex].isWhite == boardState.isWhiteTurn && boardState.pieces[toIndex].type != NONE)
        {
                std::cerr << "To index is occupied by same color\n";
                return false;
        }

        if (!boardState.canMoveTo[fromIndex][toIndex])
        {
                std::cerr << "Move is not legal\n";
                return false;
        }

        if (boardState.pieces[toIndex].type == KING)
        {
                std::cerr << "Move target is king\n";
                return false;
        }

        std::cout << "Move is legal\n";
        return true;
}

void pawnMoved(BoardState& boardState, int fromIndex, int toIndex)
{
        Piece movedPiece = boardState.pieces[fromIndex];
        if (movedPiece.type == PAWN)
        {
                int fromFile = fromIndex % 8;
                int toFile = toIndex % 8;
                int fileDistance = toFile - fromFile;
                if (boardState.enPassantSquare == toIndex && (fileDistance == 1 || fileDistance == -1))
                {
                        int capturedPawnIndex = toIndex + (movedPiece.isWhite ? 8 : -8);
                        boardState.pieces[capturedPawnIndex] = {NONE, false};
                        boardState.enPassantSquare = -1;
                }

                int fromRank = fromIndex / 8;
                int toRank = toIndex / 8;
                int rankDistance = toRank - fromRank;
                if (rankDistance == 2 || rankDistance == -2)
                {
                        boardState.enPassantSquare = fromIndex + (movedPiece.isWhite ? 8 : -8);
                }
                else
                {
                        boardState.enPassantSquare = -1;
                }

                if (toRank == 0 && movedPiece.isWhite)
                {
                        // TODO: prompt user for promotion
                        boardState.pieces[toIndex] = {QUEEN, true};
                }
                else if (toRank == 7 && !movedPiece.isWhite)
                {
                        // TODO: prompt user for promotion
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
        Piece movedPiece = boardState.pieces[fromIndex];
        if (movedPiece.type == ROOK)
        {
                if (fromIndex == 0 && !movedPiece.isWhite)
                {
                        boardState.blackCanCastleQueenside = false;
                }
                else if (fromIndex == 7 && !movedPiece.isWhite)
                {
                        boardState.blackCanCastleKingside = false;
                }
                else if (fromIndex == 56 && movedPiece.isWhite)
                {
                        boardState.whiteCanCastleQueenside = false;
                }
                else if (fromIndex == 63 && movedPiece.isWhite)
                {
                        boardState.whiteCanCastleKingside = false;
                }
        }
}

void kingMoved(BoardState& boardState, int fromIndex, int toIndex)
{
        Piece movedPiece = boardState.pieces[fromIndex];
        if (movedPiece.type == KING)
        {
                if (fromIndex == 4 && !movedPiece.isWhite)
                {
                        boardState.blackCanCastleKingside = false;
                        boardState.blackCanCastleQueenside = false;
                }
                else if (fromIndex == 60 && movedPiece.isWhite)
                {
                        boardState.whiteCanCastleKingside = false;
                        boardState.whiteCanCastleQueenside = false;
                }

                int toFile = toIndex % 8;
                int fromFile = fromIndex % 8;

                int fileDistance = toFile - fromFile;

                if (fileDistance == 2)
                {
                        if (!boardState.isWhiteTurn)
                        {
                                boardState.pieces[5] = {ROOK, false};
                                boardState.pieces[7] = {NONE, false};
                        }
                        else
                        {
                                boardState.pieces[61] = {ROOK, true};
                                boardState.pieces[63] = {NONE, false};
                        }
                }
                else if (fileDistance == -2)
                {
                        if (!boardState.isWhiteTurn)
                        {
                                boardState.pieces[3] = {ROOK, false};
                                boardState.pieces[0] = {NONE, false};
                        }
                        else
                        {
                                boardState.pieces[59] = {ROOK, true};
                                boardState.pieces[56] = {NONE, false};
                        }
                }
        }
}

void addStateToHistory(const BoardState& boardState)
{
        BoardState newState = boardState;
        newState.stateHistory.push_back(newState);
}

void movePiece(BoardState& boardState, const Board& board, int fromIndex, int toIndex)
{
        if (!checkLegality(boardState, fromIndex, toIndex))
                return;

        pawnMoved(boardState, fromIndex, toIndex);
        rookMoved(boardState, fromIndex, toIndex);
        kingMoved(boardState, fromIndex, toIndex);

        addStateToHistory(boardState);

        boardState.pieces[toIndex] = boardState.pieces[fromIndex];
        boardState.pieces[fromIndex] = {NONE, false};
        boardState.isWhiteTurn = !boardState.isWhiteTurn;
        boardState.halfMoveClock++;
        boardState.fullMoveClock += boardState.isWhiteTurn ? 1 : 0;

        std::cout << "Turn: " << (boardState.isWhiteTurn ? "White" : "Black") << "\n";
        if (boardState.enPassantSquare != -1)
        {
                //std::cout << "En passant square: " << boardState.enPassantSquare << "\n";
        }

        //printBoardState(boardState);
}
