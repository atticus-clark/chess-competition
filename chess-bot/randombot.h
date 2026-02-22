#include <random>
#include "chess.hpp"

class RandomBot {
public:
    std::string Move(const std::string& fen) {
        // get legal moves
        chess::Movelist moves;
        chess::movegen::legalmoves(moves, chess::Board(fen));

        // get random move
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, moves.size() - 1);
        auto move = moves[dist(gen)];
        
        return chess::uci::moveToUci(move);
    }
};
