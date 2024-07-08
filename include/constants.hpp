#pragma once

#include <cstdint>

namespace cnchess {
    /*
        Chinese chess board is 10 x 9,
        to speed up rules checking, I added 2 lines for both the top, left, bottom and right sides.
    */
    inline constexpr int32_t BOARD_ROW_LEN = 14;
    inline constexpr int32_t BOARD_COL_LEN = 13;
    inline constexpr int32_t BOARD_ACTUAL_ROW_LEN = 10;
    inline constexpr int32_t BOARD_ACTUAL_COL_LEN = 9;
    inline constexpr int32_t BOARD_ACTUAL_ROW_BEGIN = 2;
    inline constexpr int32_t BOARD_ACTUAL_COL_BEGIN = 2;

    // if a pawn has crossed the faced river, then he can move forward, left or right.
    inline constexpr int32_t BOARD_RIVER_UP = BOARD_ACTUAL_ROW_BEGIN + 4;
    inline constexpr int32_t BOARD_RIVER_DOWN = BOARD_ACTUAL_ROW_BEGIN + 5;

    // piece: general and advisor must stay within the 9 palace.
    inline constexpr int32_t BOARD_9_PALACE_UP_TOP     = BOARD_ACTUAL_ROW_BEGIN;
    inline constexpr int32_t BOARD_9_PALACE_UP_BOTTOM  = BOARD_ACTUAL_ROW_BEGIN + 2;
    inline constexpr int32_t BOARD_9_PALACE_UP_LEFT    = BOARD_ACTUAL_COL_BEGIN + 3;
    inline constexpr int32_t BOARD_9_PALACE_UP_RIGHT   = BOARD_ACTUAL_COL_BEGIN + 5;

    inline constexpr int32_t BOARD_9_PALACE_DOWN_TOP     = BOARD_ACTUAL_ROW_BEGIN + 7;
    inline constexpr int32_t BOARD_9_PALACE_DOWN_BOTTOM  = BOARD_ACTUAL_ROW_BEGIN + 9;
    inline constexpr int32_t BOARD_9_PALACE_DOWN_LEFT    = BOARD_ACTUAL_COL_BEGIN + 3;
    inline constexpr int32_t BOARD_9_PALACE_DOWN_RIGHT   = BOARD_ACTUAL_COL_BEGIN + 5;

    // The max number of steps a player can take in a single turn.
    inline constexpr int32_t MAX_ONE_SIDE_POSSIBLE_MOVES_LEN = 256;

    // default AI difficulty.
    inline constexpr uint8_t DEFAULT_AI_SEARCH_DEPTH = 4;
};
