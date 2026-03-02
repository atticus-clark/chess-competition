#include "negamax.h"

std::string NegaMax::Move(const std::string& fen, uint depth) {
    // TODO: set up move timer

    // Check if there is no move to make //
    if(0 == depth) {
        std::cout << "ERROR: Called NegaMax::GetMove() with a depth of 0!\n";
        return "";
    }

    board.setFen(fen); // set up board for NegaMax loop
    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);
    if(0 == moves.size()) { return ""; } // separate from above check to reduce calls to legalmoves()

    // Traverse all moves //
    int bestMoveIndex; // track best move bc we need to return it later
    int bestScore = std::numeric_limits<int>::min();
    for(int i = 0; i < moves.size(); i++) {
        board.makeMove(moves[i]);
        int score = -Search(depth-1, std::numeric_limits<int>::min()+1, std::numeric_limits<int>::max());
        board.unmakeMove(moves[i]);
        if(score > bestScore) {
            bestScore = score;
            bestMoveIndex = i;
        }
    }

    return chess::uci::moveToUci(moves[bestMoveIndex]);
}

// Based on pseudocode from chessprogramming.org
// https://www.chessprogramming.org/Alpha-Beta#Negamax_Framework
int NegaMax::Search(uint depth, int alpha, int beta) {
    // Figure out if it's time to evaluate yet //
    if(0 == depth) { return Evaluate(); }

    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);
    if(0 == moves.size()) { return Evaluate(); } // separate from above check to reduce calls to legalmoves()

    // Traverse all moves //
    int bestScore = std::numeric_limits<int>::min();
    for(int i = 0; i < moves.size(); i++) {
        board.makeMove(moves[i]);
        int score = -Search(depth-1, -beta, -alpha);
        board.unmakeMove(moves[i]);

        if(score > bestScore) {
            bestScore = score;
            if(score > alpha) { alpha = score; }
        }
        if(score >= beta) { i = moves.size(); } // exit the loop
    }

    return bestScore;
}

int NegaMax::Evaluate() {
    return eval.Evaluate(board);
}
