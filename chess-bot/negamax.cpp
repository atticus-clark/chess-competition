#include "negamax.h"

std::string NegaMax::Move(const std::string& fen, int timeLimitMS) {
    // Reset iterative deepening stuff //
    startTime = std::chrono::steady_clock::now();
    nodeCount = 0;
    timeUp = false;
    timeBudgetMS = timeLimitMS * BUDGET_PERCENT / 100;

    // Get legal moves //
    board.setFen(fen);
    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);
    if(0 >= moves.size()) { return ""; } // no legal moves to make, only acceptable time to return nothing

    /*
    // Generate captures first and order them by MVV-LVA for better pruning //
    board.setFen(fen);
    chess::Movelist moves;
    chess::movegen::legalmoves<chess::movegen::MoveGenType::CAPTURE>(moves, board);
    OrderCaptures(moves);

    // Generate quiet moves second (no move ordering implemented) //
    chess::movegen::legalmoves<chess::movegen::MoveGenType::QUIET>(moves, board);
    if(0 >= moves.size()) { return ""; } // no legal moves to make, only acceptable time to return nothing
    */

    // Search each depth until time runs out //
    int moveToPlayIndex = 0; // fallback move (always provide a legal move, even if depth 1 times out)
    for(int depth = 1; depth <= MAX_DEPTH; depth++) {
        // First layer of search here so the best move can be tracked //
        int bestMoveIndex;
        int bestScore = -eval.INF;
        
        for(int i = 0; i < moves.size(); i++) {
            board.makeMove(moves[i]);
            int score = -Search(depth-1, -eval.INF, eval.INF);

            if(timeUp) { return chess::uci::moveToUci(moves[moveToPlayIndex]); } // time is up, stop searching

            board.unmakeMove(moves[i]);
            if(score > bestScore) {
                bestScore = score;
                bestMoveIndex = i;
            }
        }

        // Update move to play based on most recent depth of iterative deepening //
        moveToPlayIndex = bestMoveIndex; // depth was completed, update result
        if(std::abs(bestScore) >= eval.MATE - MAX_DEPTH) { break; } // forced mate found, no need to search further
    }

    return chess::uci::moveToUci(moves[moveToPlayIndex]);
}

// Based on pseudocode from chessprogramming.org
// https://www.chessprogramming.org/Alpha-Beta#Negamax_Framework
int NegaMax::Search(int depth, int alpha, int beta) {
    // Check if time is up //
    ++nodeCount;
    if((nodeCount & 1023) == 0) { // only check time every 1024 nodes
        CheckTime();
        if(timeUp) { return 0; } // time is up (score will be discarded)
    }
    
    // Determine if it's time to evaluate yet //
    if(0 == depth) { return Quiescence(MAX_DEPTH_QUIESCENCE, alpha, beta); }

    // Get legal moves //
    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);
    if(0 >= moves.size()) { return eval.Evaluate(board); }

    /*
    // Generate captures first and order them by MVV-LVA for better pruning //
    chess::Movelist moves;
    chess::movegen::legalmoves<chess::movegen::MoveGenType::CAPTURE>(moves, board);
    OrderCaptures(moves);

    // Generate quiet moves second (no move ordering implemented) //
    chess::movegen::legalmoves<chess::movegen::MoveGenType::QUIET>(moves, board);
    if(0 >= moves.size()) { return eval.Evaluate(board); } // no legal moves to make, only acceptable time to return nothing
    */

    // Traverse all moves //
    int bestScore = std::numeric_limits<int>::min();
    for(int i = 0; i < moves.size(); i++) {
        board.makeMove(moves[i]);
        int score = -Search(depth-1, -beta, -alpha);

        if(timeUp) { return 0; } // time is up, stop searching

        board.unmakeMove(moves[i]);
        if(score > bestScore) {
            bestScore = score;
            if(score > alpha) { alpha = score; }
        }
        if(score >= beta) { i = moves.size(); } // exit the loop
    }

    return bestScore;
}

void NegaMax::CheckTime() {
    int timeElapsedMS = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - startTime).count();
    if(timeElapsedMS >= timeBudgetMS) { timeUp = true; }
}

int NegaMax::Quiescence(int depth, int alpha, int beta) {
    // Initial checks (depth, alpha, beta) //
    int standPat = eval.Evaluate(board); // score if we choose not to capture; serves as lower bound for quiescence search
    if(0 == depth) { return standPat; } // cap quiescence search depth to prevent search explosions
    if(standPat >= beta) { return beta; } // no need to capture, position is already excellent
    if(standPat > alpha) { alpha = standPat; } // raise lower bound

    // Generate capturing moves only //
    chess::Movelist captures;
    chess::movegen::legalmoves<chess::movegen::MoveGenType::CAPTURE>(captures, board);

    // Order captures by MVV-LVA for better pruning //
    OrderCaptures(captures);

    // Iterate through all captures to find the best one //
    for(int i = 0; i < captures.size(); i++) {
        board.makeMove(captures[i]);
        int score = -Quiescence(depth-1, -beta, -alpha);
        board.unmakeMove(captures[i]);

        if(score >= beta) { return beta; } // beta cutoff
        if(score > alpha) { alpha = score; } // found a better capture
    }

    return alpha;
}

void NegaMax::OrderCaptures(chess::Movelist& captures) {
    std::sort(captures.begin(), captures.end(),
        [&](const chess::Move& a, const chess::Move& b) {
            return MVVLVA(a) > MVVLVA(b); }
    );
}

int NegaMax::MVVLVA(chess::Move capture) {
    int victim = eval.PieceValue(board.at(capture.to()).type()); // want this to be high
    int attacker = eval.PieceValue(board.at(capture.from()).type()); // want this to be low

    return victim * 10 - attacker;
}
