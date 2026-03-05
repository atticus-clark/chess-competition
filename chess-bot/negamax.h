#include <chrono>
#include <iostream>
#include <limits>
#include <stack>
#include "chess.hpp"
#include "evaluation.h"

class NegaMax {
public:
    std::string Move(const std::string& fen, int timeLimitMS);

private:
    // Evaluation
    Eval eval;
    chess::Board board;

    // Iterative Deepening
    const int MAX_DEPTH = 16;
    const int MAX_DEPTH_QUIESCENCE = 6;
    const int BUDGET_PERCENT = 85; // budget to only use 85% of provided time to deal with OS jitter

    int nodeCount;
    bool timeUp;
    std::chrono::steady_clock::time_point startTime;

    int timeBudgetMS;
    
    // Functions
    int Search(int depth, int alpha, int beta);

    void CheckTime();

    int Quiescence(int depth, int alpha, int beta);
    void OrderCaptures(chess::Movelist& captures);
    int MVVLVA(chess::Move capture);
};
