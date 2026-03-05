#pragma once
#include <string>

namespace ChessSimulator {
/**
 * @brief Move a piece on the board
 *
 * @param fen The board as FEN
 * @return std::string The move as UCI
 */
std::string Move(std::string fen, int timeLimitMS = 10000);
} // namespace ChessSimulator