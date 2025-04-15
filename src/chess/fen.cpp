#include <iostream>
#include <string>

#include "movement.h"
#include "gamestate.h"

int applyFEN(const std::string& fen, BoardState& boardState)
{
        int numKings = 0;
        boardState.pieces.fill({NONE, false}); // Clear the board

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

