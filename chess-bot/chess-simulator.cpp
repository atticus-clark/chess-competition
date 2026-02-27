#include "chess-simulator.h"
// disservin's lib. drop a star on his hard work!
// https://github.com/Disservin/chess-library
#include "chess.hpp"
#include <random>

#include "randombot.h"
#include "negamax.h"
#include "mcts.h"

using namespace ChessSimulator;

RandomBot randomBot;
NegaMax negamax;
MCTS mcts;

std::string ChessSimulator::Move(std::string fen) {
  // create your board based on the board string following the FEN notation
  // search for the best move using minimax / monte carlo tree search /
  // alpha-beta pruning / ... try to use nice heuristics to speed up the search
  // and have better results return the best move in UCI notation you will gain
  // extra points if you create your own board/move representation instead of
  // using the one provided by the library
  
  /*
  // Code for alternating chessbots for MinMax assignment
  chess::Board board(fen);
  if(board.sideToMove() == chess::Color::WHITE) { return negamax.Move(fen); }
  else if(board.sideToMove() == chess::Color::BLACK) { return randomBot.Move(fen); }
  else { return ""; } // chess::Color::NONE is a thing for some reason, so this handles that
  */

  /*
  // Code for alternating chessbots for MCTS assignment
  chess::Board board(fen);
  if(board.sideToMove() == chess::Color::WHITE) { return mcts.Search(fen); }
  else if(board.sideToMove() == chess::Color::BLACK) { return randomBot.Move(fen); }
  else { return ""; } // chess::Color::NONE is a thing for some reason, so this handles that
  */

  return mcts.Search(fen);
}
