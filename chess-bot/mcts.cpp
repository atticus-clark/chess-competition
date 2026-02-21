#include "mcts.h"
#include <iostream>

std::string MCTS::mctsSearch(const std::string& fen, int iterations) {
    MCTSNode* root = new MCTSNode(fen);

    for(int i = 0; i < iterations; i++) {
        // 1) Selection
        MCTSNode* leaf = select(root);

        // 2) Expansion
        if(!isTerminal(leaf->gameState)) {
            expand(leaf);
            leaf = leaf->children[0];
        }

        // 3) Simulation
        double rolloutResult = simulate(leaf);

        // 4) Backpropagation
        backpropagate(leaf, rolloutResult);
    }

    // return bestChild(root)->gameState.lastMove();
    return "";
}

// Select child with best UCB1 score.
MCTSNode* MCTS::select(MCTSNode* node) {
    while(!node->children.empty()) {
        node = *std::max_element(
            node->children.begin(), node->children.end(),
            [](MCTSNode* a, MCTSNode* b) { return a->ucb() < b->ucb(); }
        );
    }

    return node;
}

void MCTS::expand(MCTSNode* node) {
    // Generate legal moves from this position.
    chess::Movelist moves;
    chess::movegen::legalmoves(moves, chess::Board(node->gameState));
    
    // Assign each legal move as a child node.
    chess::Board board;
    for(int i = 0; i < moves.size(); i++) {
        board.setFen(node->gameState);
        board.makeMove(moves[i]);
        MCTSNode* child = new MCTSNode(board.getFen(), node);
        node->children.push_back(child);
    }
}

// Rollout: Play a random game to completion and return the game's result.
// 0.0 for loss, 0.5 for tie, 1.0 for win.
double MCTS::simulate(MCTSNode* node) {
    chess::Board board;
    chess::Movelist moves;

    // RNG
    std::random_device rd;
    std::mt19937 gen(rd());
    
    while(!isTerminal(board.getFen())) {
        // Get legal moves for current position
        moves.clear();
        chess::movegen::legalmoves(moves, board);

        // get random move
        std::uniform_int_distribution<> dist(0, moves.size() - 1);
        //chess::Move move = moves[dist(gen)];

        int generated = dist(gen);
        chess::Move move = moves[generated];
        std::cout << generated << std::endl;

        //
    }
}

//
void MCTS::backpropagate(MCTSNode* node, double result) {

}

//
// * MCTS::bestChild(MCTSNode* node) {}
