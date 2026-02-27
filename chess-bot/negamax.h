#include <iostream>
#include <limits>
#include "chess.hpp"
#include "evaluation.h"

class NegaMax {
public:
    // Even numbers are preferred for search depth so your last depth is your opponent's moves.
    std::string Move(const std::string& fen, uint depth = 2);

private:
    chess::Board board;
    Evaluation eval;
    // TODO: time limit
    // TODO: start time
    
    int Search(uint depth, int alpha, int beta);
    int Evaluate();
};
