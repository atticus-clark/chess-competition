// Implementation of the Simplified Evaluation Function by Tomasz Michniewski
// https://www.chessprogramming.org/Simplified_Evaluation_Function
// Implementation based (very loosely) on Python implementation by Andrew Healey
// https://healeycodes.com/building-my-own-chess-engine

#include <limits>
#include "chess.hpp"

class Eval {
public:
    int Evaluate(chess::Board board) {
        // game over checks
        chess::GameResult result = board.isGameOver().second;
        chess::Color sideToMove = board.sideToMove();
        //if(chess::GameResult::DRAW == result) { return 0; }
        if(chess::GameResult::LOSE == result) {
            return sideToMove == chess::Color::WHITE ? std::numeric_limits<int>::min()+1 : std::numeric_limits<int>::max();
        }

        bool endgame = IsEndgame(board);
        int score = 0;
        chess::Piece piece;

        for(int i = 0; i < 64; i++) {
            piece = board.at(i);
            switch(piece.internal()) {
                case chess::Piece::WHITEPAWN:
                    score += VALUE_PAWN + PST_PAWN_WHITE[i];
                    break;
                case chess::Piece::BLACKPAWN:
                    score -= VALUE_PAWN + PST_PAWN_BLACK[i];
                    break;
                case chess::Piece::WHITEKNIGHT:
                    score += VALUE_KNIGHT + PST_KNIGHT[i];
                    break;
                case chess::Piece::BLACKKNIGHT:
                    score -= VALUE_KNIGHT + PST_KNIGHT[i];
                    break;
                case chess::Piece::WHITEBISHOP:
                    score += VALUE_BISHOP + PST_BISHOP_WHITE[i];
                    break;
                case chess::Piece::BLACKBISHOP:
                    score -= VALUE_BISHOP + PST_BISHOP_BLACK[i];
                    break;
                case chess::Piece::WHITEROOK:
                    score += VALUE_ROOK + PST_ROOK_WHITE[i];
                    break;
                case chess::Piece::BLACKROOK:
                    score -= VALUE_ROOK + PST_ROOK_BLACK[i];
                    break;
                case chess::Piece::WHITEQUEEN:
                    score += VALUE_QUEEN + PST_QUEEN[i];
                    break;
                case chess::Piece::BLACKQUEEN:
                    score -= VALUE_QUEEN + PST_QUEEN[i];
                    break;
                case chess::Piece::WHITEKING:
                    score += VALUE_KING + (endgame ? PST_KING_WHITE_END[i] : PST_KING_WHITE[i]);
                    break;
                case chess::Piece::BLACKKING:
                    score -= VALUE_KING + (endgame ? PST_KING_BLACK_END[i] : PST_KING_BLACK[i]);
                    break;
                default: break;
            }
        }
        
        return sideToMove == chess::Color::WHITE ? score : -score;
    }

    bool IsEndgame(chess::Board board) {
        // Michniewski's definition
        // 1) Both sides have no queens
        // 2) Each side that has a queen has one minor piece max
        // where minor piece == knight or bishop

        chess::Bitboard bitboard;
        bool wQueens = false, bQueens = false;
        int wMinors = 0, bMinors = 0;

        // get number of queens and minor pieces for each side
        bitboard = board.pieces(chess::PieceType::QUEEN, chess::Color::WHITE);
        wQueens = !bitboard.empty();
        bitboard = board.pieces(chess::PieceType::QUEEN, chess::Color::BLACK);
        bQueens = !bitboard.empty();
        
        bitboard = board.pieces(chess::PieceType::BISHOP, chess::Color::WHITE)
            | board.pieces(chess::PieceType::KNIGHT, chess::Color::WHITE);
        wMinors = bitboard.count();
        bitboard = board.pieces(chess::PieceType::BISHOP, chess::Color::BLACK)
            | board.pieces(chess::PieceType::KNIGHT, chess::Color::BLACK);
        bMinors = bitboard.count();

        // determine endgame status
        return !((wQueens && 1 < wMinors) || (bQueens && 1 < bMinors));
    }

private:
    const int VALUE_PAWN = 100;
    const int VALUE_KNIGHT = 320;
    const int VALUE_BISHOP = 330;
    const int VALUE_ROOK = 500;
    const int VALUE_QUEEN = 900;
    const int VALUE_KING = 20000;

    const int PST_PAWN_BLACK[64] = {
         0,  0,   0,   0,  0,   0,  0,  0,
        50, 50,  50,  50, 50,  50, 50, 50,
        10, 10,  20,  30, 30,  20, 10, 10,
         5,  5,  10,  25, 25,  10,  5,  5,
         0,  0,   0,  20, 20,   0,  0,  0,
         5, -5, -10,   0,  0, -10, -5,  5,
         5, 10,  10, -20,-20,  10, 10,  5,
         0,  0,   0,   0,  0,   0,  0,  0
    };

    const int PST_PAWN_WHITE[64] = {
         0,  0,   0,   0,   0,   0,  0,  0,
         5, 10,  10, -20, -20,  10, 10,  5,
         5, -5, -10,   0,   0, -10, -5,  5,
         0,  0,   0,  20,  20,   0,  0,  0,
         5,  5,  10,  25,  25,  10,  5,  5,
        10, 10,  20,  30,  30,  20, 10, 10,
        50, 50,  50,  50,  50,  50, 50, 50,
         0,  0,   0,   0,   0,   0,  0,  0
    };

    const int PST_KNIGHT[64] = {
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20,   0,   0,   0,   0, -20, -40,
        -30,   0,  10,  15,  15,  10,   0, -30,
        -30,   5,  15,  20,  20,  15,   5, -30,
        -30,   0,  15,  20,  20,  15,   0, -30,
        -30,   5,  10,  15,  15,  10,   5, -30,
        -40, -20,   0,   5,   5,   0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50
    };

    const int PST_BISHOP_BLACK[64] = {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10,   0,   0,   0,   0,   0,   0, -10,
        -10,   0,   5,  10,  10,   5,   0, -10,
        -10,   5,   5,  10,  10,   5,   5, -10,
        -10,   0,  10,  10,  10,  10,   0, -10,
        -10,  10,  10,  10,  10,  10,  10, -10,
        -10,   5,   0,   0,   0,   0,   5, -10,
        -20, -10, -10, -10, -10, -10, -10, -20
    };

    const int PST_BISHOP_WHITE[64] = {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10,   5,   0,   0,   0,   0,   5, -10,
        -10,  10,  10,  10,  10,  10,  10, -10,
        -10,   0,  10,  10,  10,  10,   0, -10,
        -10,   5,   5,  10,  10,   5,   5, -10,
        -10,   0,   5,  10,  10,   5,   0, -10,
        -10,   0,   0,   0,   0,   0,   0, -10,
        -20, -10, -10, -10, -10, -10, -10, -20
    };

    const int PST_ROOK_BLACK[64] = {
         0,  0,  0,  0,  0,  0,  0,  0,
         5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
         0,  0,  0,  5,  5,  0,  0,  0
    };

    const int PST_ROOK_WHITE[64] = {
         0,  0,  0,  5,  5,  0,  0,  0,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
         5, 10, 10, 10, 10, 10, 10,  5,
         0,  0,  0,  0,  0,  0,  0,  0
    };

    const int PST_QUEEN[64] = {
        -20,-10, -10, -5, -5, -10, -10, -20,
        -10,  0,   0,  0,  0,   0,   0, -10,
        -10,  0,   5,  5,  5,   5,   0, -10,
         -5,  0,   5,  5,  5,   5,   0,  -5,
          0,  0,   5,  5,  5,   5,   0,  -5,
        -10,  5,   5,  5,  5,   5,   0, -10,
        -10,  0,   5,  0,  0,   0,   0, -10,
        -20,-10, -10, -5, -5, -10, -10, -20
    };

    const int PST_KING_BLACK[64] = {
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -10, -20, -20, -20, -20, -20, -20, -10,
         20,  20,   0,   0,   0,   0,  20,  20,
         20,  30,  10,   0,   0,  10,  30,  20
    };

    const int PST_KING_WHITE[64] = {
         20,  30,  10,   0,   0,  10,  30,  20,
         20,  20,   0,   0,   0,   0,  20,  20,
        -10, -20, -20, -20, -20, -20, -20, -10,
         20, -30, -30, -40, -40, -30, -30, -20,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30
    };

    const int PST_KING_BLACK_END[64] = {
        -50, -40, -30, -20, -20, -30, -40, -50,
        -30, -20, -10,   0,   0, -10, -20, -30,
        -30, -10,  20,  30,  30,  20, -10, -30,
        -30, -10,  30,  40,  40,  30, -10, -30,
        -30, -10,  30,  40,  40,  30, -10, -30,
        -30, -10,  20,  30,  30,  20, -10, -30,
        -30, -30,   0,   0,   0,   0, -30, -30,
        -50, -30, -30, -30, -30, -30, -30, -50
    };

    const int PST_KING_WHITE_END[64] = {
         50, -30, -30, -30, -30, -30, -30, -50,
        -30, -30,   0,   0,   0,   0, -30, -30,
        -30, -10,  20,  30,  30,  20, -10, -30,
        -30, -10,  30,  40,  40,  30, -10, -30,
        -30, -10,  30,  40,  40,  30, -10, -30,
        -30, -10,  20,  30,  30,  20, -10, -30,
        -30, -20, -10,   0,   0, -10, -20, -30,
        -50, -40, -30, -20, -20, -30, -40, -50
    };
};
