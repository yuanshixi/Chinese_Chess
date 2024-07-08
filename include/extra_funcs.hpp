#pragma once

#include "board.hpp"

namespace cnchess {
    // every one can only move his/her own pieces, not the enemy's.
    bool check_is_this_your_piece(const ChessBoard& cb, const MoveNode& move, PieceSide side);

    // if no one wins, return PS_EXTRA.
    PieceSide check_winner(const ChessBoard& cb);
};
