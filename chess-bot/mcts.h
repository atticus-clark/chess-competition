#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <vector>
#include "chess.hpp"

// Built from slides code: https://gameguild.gg/p/ai4games2/week-05

class MCTSNode {
public:
    MCTSNode* parent;
    std::vector<MCTSNode*> children;
    double wins; // draws count as 0.5 wins
    int visits;
    
    std::string state; // board state in FEN
    std::string move; // move made to get here in UCI

    MCTSNode(std::string fen, std::string uci = "", MCTSNode* parentNode = nullptr)
        : state(fen), move(uci), parent(parentNode), wins(0.0), visits(0) {}
    ~MCTSNode() {
        while(0 < children.size()) {
            delete children.back();
            children.pop_back();
        }
    }

    bool isLeaf() { return children.empty(); }

    double UCB(double C = std::sqrt(2.0));
    void DeleteOtherChildren(const MCTSNode* save);
};

class MCTS {
public:
    ~MCTS() { if(nullptr != root) { delete root; } }

    std::string Move(const std::string& fen, int64_t timeLimitMS = 9990);

private:
    MCTSNode* root = nullptr;

    // MCTS steps //
    MCTSNode* Select(MCTSNode* node);
    MCTSNode* Expand(MCTSNode* node);
    double Simulate(MCTSNode* node);
    void Backpropagate(MCTSNode* node, double result);

    // Helpers //
    MCTSNode* BestChild(const MCTSNode* root);
    void ReuseTree(const std::string& fen);
    int NumNodes(const MCTSNode* node);
};
