#include "mcts.h"

// ---------------------------------------- MCTSNode ----------------------------------------//

// C == sqrt(2) generally gives a good balance between exploitation and exploration
// C == 0 yields pure exploitation, C > sqrt(2) yields more exploration
double MCTSNode::UCB(double C) {
    if(0 >= visits) { return std::numeric_limits<double>::max(); }

    // UCB1 = (wins/visits) + C*sqrt[ln(parentVisits)/visits]
    return wins / visits + std::sqrt(std::log(parent->visits) / visits) * C;
}

void MCTSNode::DeleteOtherChildren(const MCTSNode* save) {
    while(1 < children.size()) {
        if(save == children.back()) {
            delete children[0];
            children[0] = children.back();
        }
        else { delete children.back(); }
        children.pop_back();
    }
}

// ---------------------------------------- MCTS (process) ---------------------------------------- //

std::string MCTS::Search(const std::string& fen) {
    // Attempt to reuse tree
    int iterations = ReuseTree(fen);

    MCTSNode* leaf;
    for(int i = 0; i < iterations; i++) {
        leaf = Select(root);
        leaf = Expand(leaf);
        double rolloutResult = Simulate(leaf);
        Backpropagate(leaf, rolloutResult);
    }

    // Return move to make
    root = BestChild(root);
    return root->move;
}

// Select child with best UCB1 score.
MCTSNode* MCTS::Select(MCTSNode* node) {
    while(!node->children.empty()) {
        node = *std::max_element(
            node->children.begin(), node->children.end(),
            [](MCTSNode* a, MCTSNode* b) { return a->UCB() < b->UCB(); }
        );
    }

    return node;
}

// Assumes that node has no children (should be chosen by Select()).
MCTSNode* MCTS::Expand(MCTSNode* node) {
    // Generate legal moves from this position
    chess::Board board(node->state);
    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);
    if(0 >= moves.size()) { return node; }

    // Assign each legal move as a child node.
    for(int i = 0; i < moves.size(); i++) {
        board.makeMove(moves[i]);
        MCTSNode* child = new MCTSNode(board.getFen(), chess::uci::moveToUci(moves[i]), node);
        node->children.push_back(child);
        board.unmakeMove(moves[i]);
    }

    return node->children[0];
}

// Rollout: Play a random game to completion and return the game's result.
// 0.0 for loss, 0.5 for tie, 1.0 for win.
double MCTS::Simulate(MCTSNode* node) {
    // Set up RNG
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 217);

    // "Why 217?" 218 is the known maximum number of possible moves from a reachable position:
    // https://chess.stackexchange.com/questions/4490/maximum-possible-movement-in-a-turn

    // Set up board state
    chess::Board board(node->state);
    chess::Color perspective = board.sideToMove(); // for detecting win/loss later
    chess::Movelist moves;

    // Play out a random game
    chess::GameResult result = board.isGameOver().second;
    while(chess::GameResult::NONE == result) {
        moves.clear();
        chess::movegen::legalmoves(moves, board);
        board.makeMove(moves[dist(gen) % moves.size()]);
        result = board.isGameOver().second;
    }

    // Return game result
    if(chess::GameResult::DRAW == result) { return 0.5; }
    else {
        // In this case, result == LOSE; check whose turn it is.
        // these seem like they're backwards but the chessbot plays
        // noticably better with them this way so ig this is right
        if(board.sideToMove() == perspective) { return 1.0; }
        else { return 0.0; }
    }
}

// Update visits and wins back up the tree.
void MCTS::Backpropagate(MCTSNode* node, double result) {
    while(nullptr != node) {
        ++node->visits;
        node->wins += result;
        result = 1.0 - result; // flip result; your win is your parent's loss!
        node = node->parent;
    }
}

// ---------------------------------------- MCTS (helper) ---------------------------------------- //

// Select the best child as the move to play.
MCTSNode* MCTS::BestChild(const MCTSNode* root) {
    return *std::max_element(
        root->children.begin(), root->children.end(),
        [](MCTSNode* a, MCTSNode* b) {
            return a->visits < b->visits;
        }
    );

    // This uses the "most visits" strategy.
    // Other strategies include "highest win rate" (max[wins/visits])
    // and "robust child" (fusion of the other two strategies).
}

// Attempt to reuse the MCTS tree from last move.
// If that isn't possible, generate a new tree.
int MCTS::ReuseTree(const std::string& fen) {
    // number of iterations to perform when creating
    // a new tree vs. when reusing the tree
    const int ITERATIONS_NEW = 5000;
    const int ITERATIONS_REUSED = 500;

    // no tree exists, create new tree
    if(nullptr == root) {
        root = new MCTSNode(fen);
        return ITERATIONS_NEW;
    }

    // check if one of the child nodes can become reused tree
    for(MCTSNode* child : root->children) {
        if(fen == child->state) {
            child->parent = nullptr;
            root->DeleteOtherChildren(child);
            root = child;
            return ITERATIONS_REUSED;
        }
    }

    // opponent's move didn't match any children; create new tree
    root = new MCTSNode(fen);
    return ITERATIONS_NEW;
}
