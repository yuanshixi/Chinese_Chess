#pragma once

#include <cstdint>
#include "board.hpp"

namespace cnchess {
    /* 
        calculate a chess board's score. 
        upper side value is negative, down side is positive.
    */
    int32_t board_calc_score(const ChessBoard& cb);

    /* 
        gen best move for one side. 
        searchDepth is used as difficulty rank, the bigger it is, the more time the generation costs.
        give param enum PieceSide: PS_EXTRA to this function is meaningless, you will always get a struct MoveNode object with {0, 0, 0, 0}.
    */
    MoveNode gen_best_move(ChessBoard& cb, PieceSide side, uint8_t searchDepth);
};
