#pragma once

#include <cstdint>
#include "constants.hpp"

namespace cnchess {
    // piece side.
    enum PieceSide{
        PS_UP,         // upper side player.
        PS_DOWN,       // down side player.
        PS_EXTRA       // neither upper nor down, for example: empty or out of chess board.
    };

    // piece type.
    enum PieceType{
        PT_PAWN,       // pawn.
        PT_CANNON,     // cannon.
        PT_ROOK,       // rook.
        PT_KNIGHT,     // knight.
        PT_BISHOP,     // bishop.
        PT_ADVISOR,    // advisor.
        PT_GENERAL,    // general.
        PT_EMPTY,      // empty here.
        PT_OUT         // out of chess board.
    };

    // piece, it is only used as an integer, so there's no need to use enum class(C++11).
    enum Piece{
        P_UP,              // upper pawn.
        P_UC,              // upper cannon.
        P_UR,              // upper rook.
        P_UN,              // upper knight.
        P_UB,              // upper bishop.
        P_UA,              // upper advisor.
        P_UG,              // upper general.
        P_DP,              // down pawn.
        P_DC,              // down cannon.
        P_DR,              // down rook.
        P_DN,              // down knight.
        P_DB,              // down bishop.
        P_DA,              // down advisor.
        P_DG,              // down general.
        P_EE,              // empty.
        P_EO,              // out of chess board. used for speeding up rules checking.
        PIECE_TOTAL_LEN    // total number of pieces, you maybe never need this.
    };

    inline constexpr char pieceCharMapping[] = {
        'P',   /* upper pawn. */
        'C',   /* upper cannon. */
        'R',   /* upper rook. */
        'N',   /* upper knight. */
        'B',   /* upper bishop. */
        'A',   /* upper advisor. */
        'G',   /* upper general. */

        'p',   /* down pawn. */
        'c',   /* down cannon. */
        'r',   /* down rook. */
        'n',   /* down knight. */
        'b',   /* down bishop. */
        'a',   /* down advisor. */
        'g',   /* down general. */

        '.',   /* empty. */
        '#',   /* out of chess board. */
    };

     /* 
        a default chess board, used as a template for new board.
        P_EO is used here for speeding up rules checking.
    */
    inline constexpr Piece CHESS_BOARD_DEFAULT_TEMPLATE[BOARD_ROW_LEN][BOARD_COL_LEN] = {
        { P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO },
        { P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO },
        { P_EO, P_EO, P_UR, P_UN, P_UB, P_UA, P_UG, P_UA, P_UB, P_UN, P_UR, P_EO, P_EO },
        { P_EO, P_EO, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EO, P_EO },
        { P_EO, P_EO, P_EE, P_UC, P_EE, P_EE, P_EE, P_EE, P_EE, P_UC, P_EE, P_EO, P_EO },
        { P_EO, P_EO, P_UP, P_EE, P_UP, P_EE, P_UP, P_EE, P_UP, P_EE, P_UP, P_EO, P_EO },
        { P_EO, P_EO, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EO, P_EO },
        { P_EO, P_EO, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EO, P_EO },
        { P_EO, P_EO, P_DP, P_EE, P_DP, P_EE, P_DP, P_EE, P_DP, P_EE, P_DP, P_EO, P_EO },
        { P_EO, P_EO, P_EE, P_DC, P_EE, P_EE, P_EE, P_EE, P_EE, P_DC, P_EE, P_EO, P_EO },
        { P_EO, P_EO, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EE, P_EO, P_EO },
        { P_EO, P_EO, P_DR, P_DN, P_DB, P_DA, P_DG, P_DA, P_DB, P_DN, P_DR, P_EO, P_EO },
        { P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO },
        { P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO, P_EO },
    };

    inline constexpr PieceSide pieceSideMapping[] = {
        PS_UP, PS_UP, PS_UP, PS_UP, PS_UP, PS_UP, PS_UP,
        PS_DOWN, PS_DOWN, PS_DOWN, PS_DOWN, PS_DOWN, PS_DOWN, PS_DOWN,
        PS_EXTRA, PS_EXTRA
    };

    inline constexpr PieceType pieceTypeMapping[] = {
        PT_PAWN, PT_CANNON, PT_ROOK, PT_KNIGHT, PT_BISHOP, PT_ADVISOR, PT_GENERAL,
        PT_PAWN, PT_CANNON, PT_ROOK, PT_KNIGHT, PT_BISHOP, PT_ADVISOR, PT_GENERAL,
        PT_EMPTY, PT_OUT
    };

    inline constexpr PieceSide pieceSideReverseMapping[] = {
        PS_DOWN,     // upper side reverse is down.
        PS_UP,       // down side reverse is up.
        PS_EXTRA     // extra remains extra.
    };

    /*
        every piece's value. 
        upper side piece's value is negative, down side is positive. 
    */
    inline constexpr int32_t pieceValueMapping[] = {
        -20,       /* upper pawn. */
        -50,       /* upper cannon. */
        -100,      /* upper rook. */
        -50,       /* upper knight. */
        -10,       /* upper bishop. */
        -10,       /* upper advisor. */
        -10000,    /* upper general. */

        +20,       /* down pawn. */
        +50,       /* down cannon. */
        +100,      /* down rook. */
        +50,       /* down knight. */
        +10,       /* down bishop. */
        +10,       /* down advisor. */
        +10000,    /* down general. */
    };

    /* 
        every piece's position value on the chess board. 
        upper side piece's value is negative, down side is positive. 
    */
    inline constexpr int32_t piecePosValueMapping[][BOARD_ACTUAL_ROW_LEN][BOARD_ACTUAL_COL_LEN] = {
        /* Upper pawn. */
        {
            {   0,   0,   0,   0,   0,   0,   0,   0,   0 },
            {   0,   0,   0,   0,   0,   0,   0,   0,   0 },
            {   0,   0,   0,   0,   0,   0,   0,   0,   0 },
            {   2,   0,   2,   0,  -6,   0,   2,   0,   2 },
            {  -3,   0,  -4,   0,  -7,   0,  -4,   0,  -3 },
            { -10, -18, -22, -35, -40, -35, -22, -18, -10 },
            { -20, -27, -30, -40, -42, -40, -30, -27, -20 },
            { -20, -30, -45, -55, -55, -55, -45, -30, -20 },
            { -20, -30, -50, -65, -70, -65, -50, -30, -20 },
            {   0,   0,   0,  -2,  -4,  -2,   0,   0,   0 }
        },
        /* Upper cannon. */
        {
            {  0,  0, -1, -3, -3, -3, -1,  0,  0 },
            {  0, -1, -2, -2, -2, -2, -2, -1,  0 },
            { -1,  0, -4, -3, -5, -3, -4,  0, -1 },
            {  0,  0,  0,  0,  0,  0,  0,  0,  0 },
            {  1,  0, -3,  0, -4,  0, -3,  0,  1 },
            {  0,  0,  0,  0, -4,  0,  0,  0,  0 },
            {  0, -3, -3, -2, -4, -2, -3, -3,  0 },
            { -1, -1,  0,  5,  4,  5,  0, -1, -1 },
            { -2, -2,  0,  4,  7,  4,  0, -2, -2 },
            { -4, -4,  0,  5,  6,  5,  0, -4, -4 }
        },
        /* Upper rook. */
        {
            {  6,  -6,  -4, -12,  0,  -12,  -4,  -6,  6 },
            { -5,  -8,  -6, -12,  0,  -12,  -6,  -8, -5 },
            {  2,  -8,  -4, -12, -12, -12,  -4,  -8,  2 },
            { -4,  -9,  -4, -12, -14, -12,  -4,  -9, -4 },
            { -8, -12, -12, -14, -15, -14, -12, -12, -8 },
            { -8, -11, -11, -14, -15, -14, -11, -11, -8 },
            { -6, -13, -13, -16, -16, -16, -13, -13, -6 },
            { -6,  -8,  -7, -14, -16, -14,  -7,  -8, -6 },
            { -6, -12,  -9, -16, -33, -16,  -9, -12, -6 },
            { -6,  -8,  -7, -13, -14, -13,  -7,  -8, -6 }
        },
        /* Upper knight. */
        {
            {  0,   3,  -2,   0,  -2,   0,  -2,   3,  0 },
            {  3,  -2,  -4,  -5,  10,  -5,  -4,  -2,  3 },
            { -5,  -4, -6,   -7,  -4,  -7,  -6,  -4, -5 },
            { -4,  -6, -10,  -7, -10,  -7, -10,  -6, -4 },
            { -2, -10, -13, -14, -15, -14, -13, -10, -2 },
            { -2, -12, -11, -15, -16, -15, -11, -12, -2 },
            { -5, -20, -12, -19, -12, -19, -12, -20, -5 },
            { -4, -10, -11, -15, -11, -15, -11, -10, -4 },
            { -2,  -8, -15,  -9,  -6,  -9, -15,  -8, -2 },
            { -2,  -2,  -2,  -8,  -2,  -8,  -2,  -2, -2 }
        },
        /* Upper bishop. */
        {
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 2, 0, 0, 0, -3, 0, 0, 0, 2 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 }
        },
        /* Upper advisor. */
        {
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, -3, 0, 0, 0, 0 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 },
            { 0, 0, 0, 0,  0, 0, 0, 0, 0 }
        },
        /* Upper general. */
        {
            { 0, 0, 0, -1, -5, -1, 0, 0, 0 },
            { 0, 0, 0,  8,  8,  8, 0, 0, 0 },
            { 0, 0, 0,  9,  9,  9, 0, 0, 0 },
            { 0, 0, 0,  0,  0,  0, 0, 0, 0 },
            { 0, 0, 0,  0,  0,  0, 0, 0, 0 },
            { 0, 0, 0,  0,  0,  0, 0, 0, 0 },
            { 0, 0, 0,  0,  0,  0, 0, 0, 0 },
            { 0, 0, 0,  0,  0,  0, 0, 0, 0 },
            { 0, 0, 0,  0,  0,  0, 0, 0, 0 },
            { 0, 0, 0,  0,  0,  0, 0, 0, 0 }
        },
        /* Down pawn. */
        {
            {  0,  0,  0,  2,  4,  2,  0,  0,  0 },
            { 20, 30, 50, 65, 70, 65, 50, 30, 20 },
            { 20, 30, 45, 55, 55, 55, 45, 30, 20 },
            { 20, 27, 30, 40, 42, 40, 30, 27, 20 },
            { 10, 18, 22, 35, 40, 35, 22, 18, 10 },
            {  3,  0,  4,  0,  7,  0,  4,  0,  3 },
            { -2,  0, -2,  0,  6,  0, -2,  0, -2 },
            {  0,  0,  0,  0,  0,  0,  0,  0,  0 },
            {  0,  0,  0,  0,  0,  0,  0,  0,  0 },
            {  0,  0,  0,  0,  0,  0,  0,  0,  0 }
        },
        /* Down cannon. */
        {
            {  4, 4, 0, -5, -6, -5, 0, 4,  4 },
            {  2, 2, 0, -4, -7, -4, 0, 2,  2 },
            {  1, 1, 0, -5, -4, -5, 0, 1,  1 },
            {  0, 3, 3,  2,  4,  2, 3, 3,  0 },
            {  0, 0, 0,  0,  4,  0, 0, 0,  0 },
            { -1, 0, 3,  0,  4,  0, 3, 0, -1 },
            {  0, 0, 0,  0,  0,  0, 0, 0,  0 },
            {  1, 0, 4,  3,  5,  3, 4, 0,  1 },
            {  0, 1, 2,  2,  2,  2, 2, 1,  0 },
            {  0, 0, 1,  3,  3,  3, 1, 0,  0 },
        },
        /* Down rook. */
        {
            {  6,  8,  7, 13, 14, 13,  7,  8,  6 },
            {  6, 12,  9, 16, 33, 16,  9, 12,  6 },
            {  6,  8,  7, 14, 16, 14,  7,  8,  6 },
            {  6, 13, 13, 16, 16, 16, 13, 13,  6 },
            {  8, 11, 11, 14, 15, 14, 11, 11,  8 },
            {  8, 12, 12, 14, 15, 14, 12, 12,  8 },
            {  4,  9,  4, 12, 14, 12,  4,  9,  4 },
            { -2,  8,  4, 12, 12, 12,  4,  8, -2 },
            {  5,  8,  6, 12,  0, 12,  6,  8,  5 },
            { -6,  6,  4, 12,  0, 12,  4,  6, -6 },
        },
        /* Down knight. */
        {
            {  2,  2,  2,  8,   2,  8,  2,  2,  2 },
            {  2,  8, 15,  9,   6,  9, 15,  8,  2 },
            {  4, 10, 11, 15,  11, 15, 11, 10,  4 },
            {  5, 20, 12, 19,  12, 19, 12, 20,  5 },
            {  2, 12, 11, 15,  16, 15, 11, 12,  2 },
            {  2, 10, 13, 14,  15, 14, 13, 10,  2 },
            {  4,  6, 10,  7,  10,  7, 10,  6,  4 },
            {  5,  4,  6,  7,   4,  7,  6,  4,  5 },
            { -3,  2,  4,  5, -10,  5,  4,  2, -3 },
            {  0, -3,  2,  0,   2,  0,  2, -3,  0 },
        },
        /* Down bishop. */
        {
            {  0, 0, 0, 0, 0, 0, 0, 0,  0 },
            {  0, 0, 0, 0, 0, 0, 0, 0,  0 },
            {  0, 0, 0, 0, 0, 0, 0, 0,  0 },
            {  0, 0, 0, 0, 0, 0, 0, 0,  0 },
            {  0, 0, 0, 0, 0, 0, 0, 0,  0 },
            {  0, 0, 0, 0, 0, 0, 0, 0,  0 },
            {  0, 0, 0, 0, 0, 0, 0, 0,  0 },
            { -2, 0, 0, 0, 3, 0, 0, 0, -2 },
            {  0, 0, 0, 0, 0, 0, 0, 0,  0 },
            {  0, 0, 0, 0, 0, 0, 0, 0,  0 }
        },
        /* Down advisor. */
        {
            { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 3, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0 }
        },
        /* Down general. */
        {
            { 0, 0, 0,  0,  0,  0, 0, 0, 0 },
            { 0, 0, 0,  0,  0,  0, 0, 0, 0 },
            { 0, 0, 0,  0,  0,  0, 0, 0, 0 },
            { 0, 0, 0,  0,  0,  0, 0, 0, 0 },
            { 0, 0, 0,  0,  0,  0, 0, 0, 0 },
            { 0, 0, 0,  0,  0,  0, 0, 0, 0 },
            { 0, 0, 0,  0,  0,  0, 0, 0, 0 },
            { 0, 0, 0, -9, -9, -9, 0, 0, 0 },
            { 0, 0, 0, -8, -8, -8, 0, 0, 0 },
            { 0, 0, 0,  1,  5,  1, 0, 0, 0 }
        }
    };

    constexpr char piece_get_char(Piece p){
        return pieceCharMapping[p];
    }

    constexpr PieceSide piece_get_side(Piece p){
        return pieceSideMapping[p];
    }

    constexpr PieceType piece_get_type(Piece p){
        return pieceTypeMapping[p];
    }

    constexpr PieceSide piece_side_get_reverse(PieceSide side){
        return pieceSideReverseMapping[side];
    }

    constexpr int32_t piece_get_value(Piece p){
        return pieceValueMapping[p];
    }

    constexpr int32_t piece_get_pos_value(Piece p, int32_t r, int32_t c){
        return piecePosValueMapping[p][r][c];
    }
};
