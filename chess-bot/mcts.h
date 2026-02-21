#pragma once
#include <cmath>
#include <limits>
#include <random>
#include <vector>
#include "chess.hpp"

// Built from slides code: https://gameguild.gg/p/ai4games2/week-05

class MCTS {
public:
    //
    
    std::string mctsSearch(const std::string& fen, int iterations);
    MCTSNode* select(MCTSNode* node);
    void expand(MCTSNode* node);
    double simulate(MCTSNode* node);
    void backpropagate(MCTSNode* node, double result);
    // bestChild(MCTSNode* node);

    bool isTerminal(const std::string& fen) {
        // isGameOver().first will either give a reason for the game's end (terminal state)
        // or "none" because the game hasn't ended yet (non-terminal state)
        if(chess::GameResultReason::NONE != chess::Board(fen).isGameOver().first) { return true; }
        return false;
    }
};

class MCTSNode {
public:
    MCTSNode* parent;
    std::vector<MCTSNode*> children;

    std::string gameState;
    double wins; // a draw adds 0.5 wins
    int visits;

    MCTSNode(std::string state, MCTSNode* parentNode = nullptr) : parent(parentNode), wins(0.0), visits(0), gameState(state) {}

    // C = sqrt(2) generally gives a good balance between exploitation and exploration
    // C = 0 yields pure exploitation, C > sqrt(2) yields more exploration
    double ucb(double C = std::sqrt(2.0)) {
        if(0 >= visits) { return std::numeric_limits<double>::max(); }

        // UCB1 = (wins/visits) + C*sqrt[ln(parentVisits)/visits]
        return wins / visits + std::sqrt(std::log(parent->visits) / visits) * C;
    }

    bool isLeaf() { return children.empty(); }
};
