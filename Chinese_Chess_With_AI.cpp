/*
    @author yuanshixi
*/
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <deque>
#include <algorithm>
#include <limits>
#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#endif

/*
	Chinese chess board is 10 x 9,
	to speed up rules checking, I added 2 lines for both the top, left, bottom and right sides.
*/
constexpr int32_t BOARD_ROW_LEN = 10;
constexpr int32_t BOARD_COL_LEN =  9;
constexpr int32_t BOARD_ACTUAL_ROW_LEN = 14;
constexpr int32_t BOARD_ACTUAL_COL_LEN = 13;

constexpr int32_t BOARD_ACTUAL_ROW_BEGIN = 2;
constexpr int32_t BOARD_ACTUAL_COL_BEGIN = 2;
constexpr int32_t BOARD_ACTUAL_ROW_END = BOARD_ACTUAL_ROW_BEGIN + BOARD_ROW_LEN - 1;
constexpr int32_t BOARD_ACTUAL_COL_END = BOARD_ACTUAL_COL_BEGIN + BOARD_COL_LEN - 1;

// if a pawn has crossed the faced river, then he can move forward, left or right.
constexpr int32_t BOARD_RIVER_UP = BOARD_ACTUAL_ROW_BEGIN + 4;
constexpr int32_t BOARD_RIVER_DOWN = BOARD_ACTUAL_ROW_BEGIN + 5;

constexpr int32_t BOARD_CHU_HAN_LINE = BOARD_ACTUAL_ROW_BEGIN + (BOARD_ROW_LEN) / 2;

// piece: general and advisor must stay within the 9 palace.
constexpr int32_t BOARD_9_PALACE_UP_TOP     = BOARD_ACTUAL_ROW_BEGIN;
constexpr int32_t BOARD_9_PALACE_UP_BOTTOM  = BOARD_ACTUAL_ROW_BEGIN + 2;
constexpr int32_t BOARD_9_PALACE_UP_LEFT    = BOARD_ACTUAL_COL_BEGIN + 3;
constexpr int32_t BOARD_9_PALACE_UP_RIGHT   = BOARD_ACTUAL_COL_BEGIN + 5;

constexpr int32_t BOARD_9_PALACE_DOWN_TOP     = BOARD_ACTUAL_ROW_BEGIN + 7;
constexpr int32_t BOARD_9_PALACE_DOWN_BOTTOM  = BOARD_ACTUAL_ROW_BEGIN + 9;
constexpr int32_t BOARD_9_PALACE_DOWN_LEFT    = BOARD_ACTUAL_COL_BEGIN + 3;
constexpr int32_t BOARD_9_PALACE_DOWN_RIGHT   = BOARD_ACTUAL_COL_BEGIN + 5;

// The max number of steps a player can take in a single turn.
constexpr int32_t MAX_ONE_SIDE_POSSIBLE_MOVES_LEN = 256;

// default AI difficulty.
constexpr uint16_t DEFAULT_AI_SEARCH_DEPTH = 4;

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

constexpr char pieceCharMapping[] = {
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

constexpr PieceSide pieceSideMapping[] = {
    PS_UP, PS_UP, PS_UP, PS_UP, PS_UP, PS_UP, PS_UP,
    PS_DOWN, PS_DOWN, PS_DOWN, PS_DOWN, PS_DOWN, PS_DOWN, PS_DOWN,
    PS_EXTRA, PS_EXTRA
};

constexpr PieceType pieceTypeMapping[] = {
    PT_PAWN, PT_CANNON, PT_ROOK, PT_KNIGHT, PT_BISHOP, PT_ADVISOR, PT_GENERAL,
    PT_PAWN, PT_CANNON, PT_ROOK, PT_KNIGHT, PT_BISHOP, PT_ADVISOR, PT_GENERAL,
    PT_EMPTY, PT_OUT
};

constexpr PieceSide pieceSideReverseMapping[] = {
    PS_DOWN,     // upper side reverse is down.
    PS_UP,       // down side reverse is up.
    PS_EXTRA     // extra remains extra.
};

/*
    every piece's value. 
    upper side piece's value is negative, down side is positive. 
*/
constexpr int32_t pieceValueMapping[] = {
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
constexpr int32_t piecePosValueMapping[][BOARD_ROW_LEN][BOARD_COL_LEN] = {
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

/* 
    a default chess board, used as a template for new board.
    P_EO is used here for speeding up rules checking.
*/
constexpr Piece DEFAULT_CHESS_BOARD_DATA[BOARD_ACTUAL_ROW_LEN][BOARD_ACTUAL_COL_LEN] = {
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

// move node, reprensent a move.
struct MoveNode{
    int32_t beginRow;
    int32_t beginCol;
    int32_t endRow;
    int32_t endCol;

    MoveNode()
        : beginRow{ 0 }, beginCol{ 0 }, endRow{ 0 }, endCol{ 0 }
    {}

    MoveNode(int32_t beginRow, int32_t beginCol, int32_t endRow, int32_t endCol)
        : beginRow{ beginRow }, beginCol{ beginCol }, endRow{ endRow }, endCol{ endCol }
    {}

    bool operator==(const MoveNode& other) const noexcept {
        return beginRow == other.beginRow &&
                beginCol == other.beginCol &&
                endRow == other.endRow &&
                endCol == other.endCol;
    }

    bool operator!=(const MoveNode& other) const noexcept {
        return !(*this == other);
    }
};

// history node, used for undo the previous move.
struct HistoryNode{
    MoveNode move;
    Piece beginPiece;
    Piece endPiece;

    HistoryNode(const MoveNode& moveNode, Piece beginPiece, Piece endPiece)
        : move{ moveNode }, beginPiece{ beginPiece }, endPiece{ endPiece }
    {}
};

// chess board.
class ChessBoard{
    std::array<std::array<Piece, BOARD_ACTUAL_COL_LEN>, BOARD_ACTUAL_ROW_LEN> data;
    std::deque<HistoryNode> history;
public:
    ChessBoard(){
        clear();
    }

    Piece get(int32_t r, int32_t c) const noexcept {
        return data[r][c];
    }

    void set(int32_t r, int32_t c, Piece p) noexcept {
        data[r][c] = p;
    }

    void clear() noexcept {
        for (int32_t r = 0; r < BOARD_ACTUAL_ROW_LEN; ++r) {
            for (int32_t c = 0; c < BOARD_ACTUAL_COL_LEN; ++c) {
                data[r][c] = DEFAULT_CHESS_BOARD_DATA[r][c]; 
            }
        }

        history.clear();
    }

    void move(const MoveNode& moveNode){
        Piece beginPiece = get(moveNode.beginRow, moveNode.beginCol);
        Piece endPiece = get(moveNode.endRow, moveNode.endCol);

        // record the history.
        history.emplace_back(moveNode, beginPiece, endPiece);

        // move the pieces.
        set(moveNode.beginRow, moveNode.beginCol, P_EE);
        set(moveNode.endRow, moveNode.endCol, beginPiece);
    }

    void undo(){
        if (!history.empty()){   // if history is not empty, reset pieces and pop back.
            const HistoryNode& node = history.back();

            set(node.move.beginRow, node.move.beginCol, node.beginPiece);
            set(node.move.endRow, node.move.endCol, node.endPiece);

            history.pop_back();
        }
    }
};

using PossibleMoves = std::vector<MoveNode>;

void check_possible_move_and_insert(const ChessBoard& cb, PossibleMoves& pm, int32_t beginRow, int32_t beginCol, int32_t endRow, int32_t endCol){
    Piece beginP = cb.get(beginRow, beginCol);
    Piece endP = cb.get(endRow, endCol);

    if (endP != P_EO && piece_get_side(beginP) != piece_get_side(endP)){   // not out of chess board, and not the same side.
        pm.emplace_back(beginRow, beginCol, endRow, endCol);
    }
}

void gen_moves_pawn(const ChessBoard& cb, PossibleMoves& pm, int32_t r, int32_t c, PieceSide side){
    if (side == PS_UP){
        check_possible_move_and_insert(cb, pm,  r, c, r + 1, c);

        if (r > BOARD_RIVER_UP){    // cross the river ?
            check_possible_move_and_insert(cb, pm, r, c, r, c - 1);
            check_possible_move_and_insert(cb, pm, r, c, r, c + 1);
        }
    }
    else if (side == PS_DOWN){
        check_possible_move_and_insert(cb, pm, r, c, r - 1, c);

        if (r < BOARD_RIVER_DOWN){
            check_possible_move_and_insert(cb, pm, r, c, r, c - 1);
            check_possible_move_and_insert(cb, pm, r, c, r, c + 1);
        }
    }
}

void gen_moves_cannon_one_direction(const ChessBoard& cb, PossibleMoves& pm, int32_t r, int32_t c, int32_t rGap, int32_t cGap, PieceSide side){
    int32_t row, col;
    Piece p;

    for (row = r + rGap, col = c + cGap; ;row += rGap, col += cGap){
        p = cb.get(row, col);

        if (p == P_EE){    // empty piece, then insert it.
            pm.emplace_back(r, c, row, col);
        }
        else {   // upper piece, down piece or out of chess board, break immediately.
            break;
        }
    }

    if (p != P_EO){   // not out of chess board, check if we can add an enemy piece.
        for (row = row + rGap, col = col + cGap; ;row += rGap, col += cGap){
            p = cb.get(row, col);
        
            if (p == P_EE){    // empty, then continue search.
                continue;
            }
            else if (piece_get_side(p) == piece_side_get_reverse(side)){   // enemy piece, then insert it and break.
                pm.emplace_back(r, c, row, col);
                break;
            }
            else {    // self side piece or out of chess board, break.
                break;
            }
        }
    }
}

void gen_moves_cannon(const ChessBoard& cb, PossibleMoves& pm, int32_t r, int32_t c, PieceSide side){
    // go up, down, left, right.
    gen_moves_cannon_one_direction(cb, pm, r, c, -1, 0, side);
    gen_moves_cannon_one_direction(cb, pm, r, c, +1, 0, side);
    gen_moves_cannon_one_direction(cb, pm, r, c, 0, -1, side);
    gen_moves_cannon_one_direction(cb, pm, r, c, 0, +1, side);
}

void gen_moves_rook_one_direction(const ChessBoard& cb, PossibleMoves& pm, int32_t r, int32_t c, int32_t rGap, int32_t cGap, PieceSide side){
    int32_t row, col;
    Piece p;

    for (row = r + rGap, col = c + cGap; ;row += rGap, col += cGap){
        p = cb.get(row, col);

        if (p == P_EE){    // empty piece, then insert it.
            pm.emplace_back(r, c, row, col);
        }
        else {   // upper piece, down piece or out of chess board, break immediately.
            break;
        }
    }

    if (piece_get_side(p) == piece_side_get_reverse(side)){   // enemy piece, then insert it.
        pm.emplace_back(r, c, row, col);
    }
}

void gen_moves_rook(const ChessBoard& cb, PossibleMoves& pm, int32_t r, int32_t c, PieceSide side){
    // go up, down, left, right.
    gen_moves_rook_one_direction(cb, pm, r, c, -1, 0, side);
    gen_moves_rook_one_direction(cb, pm, r, c, +1, 0, side);
    gen_moves_rook_one_direction(cb, pm, r, c, 0, -1, side);
    gen_moves_rook_one_direction(cb, pm, r, c, 0, +1, side);
}

void gen_moves_knight(const ChessBoard& cb, PossibleMoves& pm, int32_t r, int32_t c, PieceSide side){
    Piece p;
    if ((p = cb.get(r + 1, c)) == P_EE){    // if not lame horse leg ?
        check_possible_move_and_insert(cb, pm, r, c, r + 2, c + 1);
        check_possible_move_and_insert(cb, pm, r, c, r + 2, c - 1);
    }

    if ((p = cb.get(r - 1, c)) == P_EE){
        check_possible_move_and_insert(cb, pm, r, c, r - 2, c + 1);
        check_possible_move_and_insert(cb, pm, r, c, r - 2, c - 1);
    }

    if ((p = cb.get(r, c + 1)) == P_EE){
        check_possible_move_and_insert(cb, pm, r, c, r + 1, c + 2);
        check_possible_move_and_insert(cb, pm, r, c, r - 1, c + 2);
    }

    if ((p = cb.get(r, c - 1)) == P_EE){
        check_possible_move_and_insert(cb, pm, r, c, r + 1, c - 2);
        check_possible_move_and_insert(cb, pm, r, c, r - 1, c - 2);
    }
}

void gen_moves_bishop(const ChessBoard& cb, PossibleMoves& pm, int32_t r, int32_t c, PieceSide side){
    Piece p;
    if (side == PS_UP){
        if (r + 2 <= BOARD_RIVER_UP){       // bishop can't cross river.
            if ((p = cb.get(r + 1, c + 1)) == P_EE){    // bishop can move only if Xiang Yan is empty.
                check_possible_move_and_insert(cb, pm, r, c, r + 2, c + 2);
            }

            if ((p = cb.get(r + 1, c - 1)) == P_EE){
                check_possible_move_and_insert(cb, pm, r, c, r + 2, c - 2);
            }
        }

        if ((p = cb.get(r - 1, c + 1)) == P_EE){
            check_possible_move_and_insert(cb, pm, r, c, r - 2, c + 2);
        }

        if ((p = cb.get(r - 1, c - 1)) == P_EE){
            check_possible_move_and_insert(cb, pm, r, c, r - 2, c - 2);
        }
    }
    else if (side == PS_DOWN){
        if (r - 2 >= BOARD_RIVER_DOWN){
            if ((p = cb.get(r - 1, c + 1)) == P_EE){
                check_possible_move_and_insert(cb, pm, r, c, r - 2, c + 2);
            }

            if ((p = cb.get(r - 1, c - 1)) == P_EE){
                check_possible_move_and_insert(cb, pm, r, c, r - 2, c - 2);
            }
        }

        if ((p = cb.get(r + 1, c + 1)) == P_EE){
            check_possible_move_and_insert(cb, pm, r, c, r + 2, c + 2);
        }

        if ((p = cb.get(r + 1, c - 1)) == P_EE){
            check_possible_move_and_insert(cb, pm, r, c, r + 2, c - 2);
        }
    }
}

void gen_moves_advisor(const ChessBoard& cb, PossibleMoves& pm, int32_t r, int32_t c, PieceSide side){
    if (side == PS_UP){
        if (r + 1 <= BOARD_9_PALACE_UP_BOTTOM && c + 1 <= BOARD_9_PALACE_UP_RIGHT) {   // walk diagonal lines.
            check_possible_move_and_insert(cb, pm, r, c, r + 1, c + 1);
        }

        if (r + 1 <= BOARD_9_PALACE_UP_BOTTOM && c - 1 >= BOARD_9_PALACE_UP_LEFT) {
            check_possible_move_and_insert(cb, pm, r, c, r + 1, c - 1);
        }

        if (r - 1 >= BOARD_9_PALACE_UP_TOP && c + 1 <= BOARD_9_PALACE_UP_RIGHT) {
            check_possible_move_and_insert(cb, pm, r, c, r - 1, c + 1);
        }

        if (r - 1 >= BOARD_9_PALACE_UP_TOP && c - 1 >= BOARD_9_PALACE_UP_LEFT) {
            check_possible_move_and_insert(cb, pm, r, c, r - 1, c - 1);
        }
    }
    else if (side == PS_DOWN){
        if (r + 1 <= BOARD_9_PALACE_DOWN_BOTTOM && c + 1 <= BOARD_9_PALACE_DOWN_RIGHT) {
            check_possible_move_and_insert(cb, pm, r, c, r + 1, c + 1);
        }

        if (r + 1 <= BOARD_9_PALACE_DOWN_BOTTOM && c - 1 >= BOARD_9_PALACE_DOWN_LEFT) {
            check_possible_move_and_insert(cb, pm, r, c, r + 1, c - 1);
        }

        if (r - 1 >= BOARD_9_PALACE_DOWN_TOP && c + 1 <= BOARD_9_PALACE_DOWN_RIGHT) {
            check_possible_move_and_insert(cb, pm, r, c, r - 1, c + 1);
        }

        if (r - 1 >= BOARD_9_PALACE_DOWN_TOP && c - 1 >= BOARD_9_PALACE_DOWN_LEFT) {
            check_possible_move_and_insert(cb, pm, r, c, r - 1, c - 1);
        }
    }
}

void gen_moves_general(const ChessBoard& cb, PossibleMoves& pm, int32_t r, int32_t c, PieceSide side){
    Piece p;
    int32_t row;

    if (side == PS_UP){
        if (r + 1 <= BOARD_9_PALACE_UP_BOTTOM){   // walk horizontal or vertical.
            check_possible_move_and_insert(cb, pm, r, c, r + 1, c);
        }

        if (r - 1 >= BOARD_9_PALACE_UP_TOP){
            check_possible_move_and_insert(cb, pm, r, c, r - 1, c);
        }

        if (c + 1 <= BOARD_9_PALACE_UP_RIGHT){
            check_possible_move_and_insert(cb, pm, r, c, r, c + 1);
        }

        if (c - 1 >= BOARD_9_PALACE_UP_LEFT){
            check_possible_move_and_insert(cb, pm, r, c, r, c - 1);
        }

        // check if both generals faced each other directly.
        for (row = r + 1; row <= BOARD_ACTUAL_ROW_END ;++row){
            p = cb.get(row, c);

            if (p == P_EE){
                continue;
            }
            else if (p == P_DG){
                pm.emplace_back(r, c, row, c);
                break;
            }
            else {
                break;
            }
        }
    }
    else if (side == PS_DOWN){
        if (r + 1 <= BOARD_9_PALACE_DOWN_BOTTOM){
            check_possible_move_and_insert(cb, pm, r, c, r + 1, c);
        }

        if (r - 1 >= BOARD_9_PALACE_DOWN_TOP){
            check_possible_move_and_insert(cb, pm, r, c, r - 1, c);
        }

        if (c + 1 <= BOARD_9_PALACE_DOWN_RIGHT){
            check_possible_move_and_insert(cb, pm, r, c, r, c + 1);
        }

        if (c - 1 >= BOARD_9_PALACE_DOWN_LEFT){
            check_possible_move_and_insert(cb, pm, r, c, r, c - 1);
        }

        for (row = r - 1; row >= BOARD_ACTUAL_ROW_BEGIN ;--row){
            p = cb.get(row, c);

            if (p == P_EE){
                continue;
            }
            else if (p == P_UG){
                pm.emplace_back(r, c, row, c);
                break;
            }
            else {
                break;
            }
        }
    }
}

/* 
    generate possible moves for one side. 
    you should call free() on the returned value later.
*/
PossibleMoves gen_possible_moves(const ChessBoard& cb, PieceSide side){
    PossibleMoves pm;
    pm.reserve(MAX_ONE_SIDE_POSSIBLE_MOVES_LEN);

    Piece p;
    for (int32_t r = BOARD_ACTUAL_ROW_BEGIN; r <= BOARD_ACTUAL_ROW_END; ++r) {
        for (int32_t c = BOARD_ACTUAL_COL_BEGIN; c <= BOARD_ACTUAL_COL_END; ++c){
            p = cb.get(r, c);

            if (piece_get_side(p) == side){
                switch (piece_get_type(p))
                {
                case PT_PAWN:
                    gen_moves_pawn(cb, pm, r, c, side);
                    break;
                case PT_CANNON:
                    gen_moves_cannon(cb, pm, r, c, side);
                    break;
                case PT_ROOK:
                    gen_moves_rook(cb, pm, r, c, side);
                    break;
                case PT_KNIGHT:
                    gen_moves_knight(cb, pm, r, c, side);
                    break;
                case PT_BISHOP:
                    gen_moves_bishop(cb, pm, r, c, side);
                    break;
                case PT_ADVISOR:
                    gen_moves_advisor(cb, pm, r, c, side);
                    break;
                case PT_GENERAL:
                    gen_moves_general(cb, pm, r, c, side);
                    break;
                case PT_EMPTY:
                case PT_OUT:
                default:
                    break;
                }
            }
        }
    }

    return pm;
}

/* 
	calculate a chess board's score. 
	upper side value is negative, down side is positive.
*/
int32_t board_calc_score(const ChessBoard& cb){
    int32_t totalScore = 0;

    Piece p;
    for (int32_t r = BOARD_ACTUAL_ROW_BEGIN; r <= BOARD_ACTUAL_ROW_END; ++r) {
        for (int32_t c = BOARD_ACTUAL_COL_BEGIN; c <= BOARD_ACTUAL_COL_END; ++c){
            p = cb.get(r, c);

			if (p != P_EE){
				totalScore += piece_get_value(p);
				totalScore += piece_get_pos_value(p, r - BOARD_ACTUAL_ROW_BEGIN, c - BOARD_ACTUAL_COL_BEGIN);
            }
        }
    }

    return totalScore;
}

// min-max algorithm, with alpha-beta pruning.
int32_t min_max(ChessBoard& cb, uint16_t searchDepth, int32_t alpha, int32_t beta, PieceSide side){
    if (searchDepth == 0){
        return board_calc_score(cb);
    }

    if (side == PS_UP){
        int32_t minValue = std::numeric_limits<int32_t>::max();
        PossibleMoves possibleMoves = gen_possible_moves(cb, PS_UP);

        for (const MoveNode& node : possibleMoves) {
            cb.move(node);
            minValue = std::min(minValue, min_max(cb, searchDepth - 1, alpha, beta, PS_DOWN));
            cb.undo();

            beta = std::min(beta, minValue);
            if (alpha >= beta){
                break;
            }
        }

        return minValue;
    }
    else if (side == PS_DOWN){
        int32_t maxValue = std::numeric_limits<int32_t>::min();
        PossibleMoves possibleMoves = gen_possible_moves(cb, PS_DOWN);

        for (const MoveNode& node : possibleMoves) {
            cb.move(node);
            maxValue = std::max(maxValue, min_max(cb, searchDepth - 1, alpha, beta, PS_UP));
            cb.undo();

            alpha = std::max(alpha, maxValue);
            if (alpha >= beta){
                break;
            }
        }

        return maxValue;
    }
    else {   // never need this, just for return value.
        return 0;
    }
}

/* 
    gen best move for one side. 
    searchDepth is used as difficulty rank, the bigger it is, the more time the generation costs.
    give param enum PieceSide: PS_EXTRA to this function is meaningless, you will always get a struct MoveNode object with {0, 0, 0, 0}.
*/
MoveNode gen_best_move(ChessBoard& cb, PieceSide side, uint16_t searchDepth){
    int32_t value;
    int32_t alpha = std::numeric_limits<int32_t>::min();
    int32_t beta = std::numeric_limits<int32_t>::max();

    MoveNode bestMove;

    if (side == PS_UP){
        int32_t minValue = beta;
        PossibleMoves possibleMoves = gen_possible_moves(cb, PS_UP);

        for (const MoveNode& node : possibleMoves){
            cb.move(node);
            value = min_max(cb, searchDepth, alpha, beta, PS_DOWN);
            cb.undo();

            if (value <= minValue){
                minValue = value;
                bestMove = node;
            }
        }
    }
    else if (side == PS_DOWN){
        int32_t maxValue = alpha;
        PossibleMoves possibleMoves = gen_possible_moves(cb, PS_DOWN);

        for (const MoveNode& node : possibleMoves){
            cb.move(node);
            value = min_max(cb, searchDepth, alpha, beta, PS_UP);
            cb.undo();

            if (value >= maxValue){
                maxValue = value;
                bestMove = node;
            }
        }
    }
    
    return bestMove;
}

// given move is fit for rule ? return false if not.
bool check_rule(const ChessBoard& cb, const MoveNode& moveNode){
    Piece p = cb.get(moveNode.beginRow, moveNode.beginCol);
    PossibleMoves pm = gen_possible_moves(cb, piece_get_side(p));

    return std::find(pm.cbegin(), pm.cend(), moveNode) != pm.cend();
}

// user input could represent a move ? return false if can't.
bool check_input_is_a_move(const std::string& input){
    if (input.size() < 4){
        return false;
    }

    return  (input[0] >= 'a' && input[0] <= 'i') &&
            (input[1] >= '0' && input[1] <= '9') &&
            (input[2] >= 'a' && input[2] <= 'i') &&
            (input[3] >= '0' && input[3] <= '9');
}

/*
    convert user input to a struct MoveNode object.
    you should call check_input_is_a_move() before to make sure this converting is valid.
*/
MoveNode convert_input_to_move(const std::string& input){
    MoveNode m;

    m.beginRow = 9 - (static_cast<int32_t>(input[1]) - static_cast<int32_t>('0')) + BOARD_ACTUAL_ROW_BEGIN;
    m.beginCol = static_cast<int32_t>(input[0]) - static_cast<int32_t>('a') + BOARD_ACTUAL_COL_BEGIN;
    m.endRow   = 9 - (static_cast<int32_t>(input[3]) - static_cast<int32_t>('0')) + BOARD_ACTUAL_ROW_BEGIN;
    m.endCol   = static_cast<int32_t>(input[2]) - static_cast<int32_t>('a') + BOARD_ACTUAL_COL_BEGIN;

    return m;
}

// convert a move to string.
std::string convert_move_to_str(const MoveNode& move){
    std::string buf;

    buf += static_cast<char>(move.beginCol - BOARD_ACTUAL_COL_BEGIN + 'a');
    buf += static_cast<char>(9 - (move.beginRow - BOARD_ACTUAL_ROW_BEGIN) + '0');
    buf += static_cast<char>(move.endCol - BOARD_ACTUAL_COL_BEGIN + 'a');
    buf += static_cast<char>(9 - (move.endRow - BOARD_ACTUAL_ROW_BEGIN) + '0');
    return buf;
}

// every one can only move his pieces, not the enemy's.
bool check_is_this_your_piece(const ChessBoard& cb, const MoveNode& move, PieceSide side){
    Piece p = cb.get(move.beginRow, move.beginCol);
    return piece_get_side(p) == side;
}

// if no one wins, return PS_EXTRA.
PieceSide check_winner(const ChessBoard& cb){
    bool upAlive = false;
    bool downAlive = false;

    for (int32_t r = BOARD_9_PALACE_UP_TOP; r <= BOARD_9_PALACE_UP_BOTTOM; ++r) {
        for (int32_t c = BOARD_9_PALACE_UP_LEFT; c <= BOARD_9_PALACE_UP_RIGHT; ++c) {
            if (cb.get(r, c) == P_UG) {
                upAlive = true;
                break;
            }
        }
    }

    for (int32_t r = BOARD_9_PALACE_DOWN_TOP; r <= BOARD_9_PALACE_DOWN_BOTTOM; ++r) {
        for (int32_t c = BOARD_9_PALACE_DOWN_LEFT; c <= BOARD_9_PALACE_DOWN_RIGHT; ++c) {
            if (cb.get(r, c) == P_DG) {
                downAlive = true;
                break;
            }
        }
    }

    if (upAlive && downAlive) {
        return PS_EXTRA;
    }
    else if (upAlive) {
        return PS_UP;
    }
    else {
        return PS_DOWN;
    }
}

class ConsoleColor {
    #ifdef _WIN32
    HANDLE hOutHandle;
    WORD oldColorAttrs;

    // https://stackoverflow.com/questions/17125440/c-win32-console-color
    WORD get_windows_color_attr(int color) {
        switch(color) {
            case Black: 
                return 0;
            case Blue: 
                return FOREGROUND_BLUE;
            case Green: 
                return FOREGROUND_GREEN;
            case Cyan: 
                return FOREGROUND_GREEN | FOREGROUND_BLUE;
            case Red: 
                return FOREGROUND_RED;
            case Magenta: 
                return FOREGROUND_RED | FOREGROUND_BLUE;
            case Yellow: 
                return FOREGROUND_RED | FOREGROUND_GREEN;
            case White:
                return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            case BoldBlack: 
                return 0 | FOREGROUND_INTENSITY;
            case BoldBlue: 
                return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            case BoldGreen: 
                return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            case BoldCyan: 
                return FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            case BoldRed: 
                return FOREGROUND_RED | FOREGROUND_INTENSITY;
            case BoldMagenta: 
                return FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            case BoldYellow: 
                return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            case BoldWhite:
            default:
                return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        };
    }
    #endif
public:
    enum {
        Black, 
        Red, 
        Green, 
        Yellow, 
        Blue, 
        Magenta, 
        Cyan, 
        White,
        BoldBlack, 
        BoldRed, 
        BoldGreen, 
        BoldYellow, 
        BoldBlue, 
        BoldMagenta, 
        BoldCyan, 
        BoldWhite
    };

    ConsoleColor() {
	#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	hOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hOutHandle, &csbiInfo);
	oldColorAttrs = csbiInfo.wAttributes;
	#endif
    }

    ~ConsoleColor() noexcept {
        reset();
    }

    void set(int color) noexcept {
        #ifdef __linux__
        switch(color) {
            case Black:
                std::cout << "\033[30m"; break;
            case Red:
                std::cout << "\033[31m"; break;
            case Green:
                std::cout << "\033[32m"; break;
            case Yellow:       
                std::cout << "\033[33m"; break;
            case Blue:         
                std::cout << "\033[34m"; break;
            case Magenta:      
                std::cout << "\033[35m"; break;
            case Cyan:         
                std::cout << "\033[36m"; break;
            case White:        
                std::cout << "\033[37m"; break;
            case BoldBlack:    
                std::cout << "\033[1m\033[30m"; break;
            case BoldRed:      
                std::cout << "\033[1m\033[31m"; break;
            case BoldGreen:    
                std::cout << "\033[1m\033[32m"; break;
            case BoldYellow:   
                std::cout << "\033[1m\033[33m"; break;
            case BoldBlue:     
                std::cout << "\033[1m\033[34m"; break;
            case BoldMagenta:  
                std::cout << "\033[1m\033[35m"; break;
            case BoldCyan:     
                std::cout << "\033[1m\033[36m"; break;
            case BoldWhite:  
            default:  
                std::cout << "\033[1m\033[37m"; break;
        }
        #elif defined(_WIN32)
        SetConsoleTextAttribute(hOutHandle, get_windows_color_attr(color));
        #endif
    }

    void reset() noexcept {
        #ifdef __linux__
        std::cout << "\033[0m";
        #elif defined(_WIN32)
        SetConsoleTextAttribute(hOutHandle, oldColorAttrs);
        #endif
    }
};

void draw_board(const ChessBoard& cb){
    ConsoleColor cc;
    int n = BOARD_ROW_LEN - 1;

    cc.set(ConsoleColor::BoldWhite);
    std::cout << "\n    +-------------------+\n";

    int r, c;
    for (r = BOARD_ACTUAL_ROW_BEGIN; r <= BOARD_ACTUAL_ROW_END; ++r) {
        if (r == BOARD_CHU_HAN_LINE){
            cc.set(ConsoleColor::BoldWhite);
            std::cout << "    |===================|\n";
            std::cout << "    |===================|\n";
        }

        cc.set(ConsoleColor::BoldWhite);
        std::cout << " " << n-- << "  | ";

        for (c = BOARD_ACTUAL_COL_BEGIN; c <= BOARD_ACTUAL_COL_END; ++c){
            Piece p = cb.get(r, c);

            if (piece_get_side(p) == PS_UP) {
                cc.set(ConsoleColor::BoldBlue);
                std::cout << piece_get_char(p);
                cc.set(ConsoleColor::BoldWhite);
                std::cout << " ";
            }
            else if (piece_get_side(p) == PS_DOWN) {
                cc.set(ConsoleColor::BoldRed);
                std::cout << piece_get_char(p);
                cc.set(ConsoleColor::BoldWhite);
                std::cout << " ";
            }
            else {
                std::cout << piece_get_char(p) << " ";
            }
        }

        cc.set(ConsoleColor::BoldWhite);
        std::cout << "|\n";
    }

    cc.set(ConsoleColor::BoldWhite);
    std::cout << "    +-------------------+\n";
    std::cout << "\n      a b c d e f g h i\n\n";
    cc.reset();
}

void print_help_page(){
    std::cout << "\n=======================================\n";
    std::cout << "Help Page\n\n";
    std::cout << "    1. help         - this page.\n";
    std::cout << "    2. b2e2         - input like this will be parsed as a move.\n";
    std::cout << "    3. undo         - undo the previous move.\n";
    std::cout << "    4. exit or quit - exit the game.\n";
    std::cout << "    5. remake       - remake the game.\n";
    std::cout << "    6. diff         - change the AI difficulty.\n";
    std::cout << "    7. advice       - give me a best move.\n\n";
    std::cout << "  The characters on the board have the following relationships: \n\n";
    std::cout << "    P -> AI side pawn.\n";
    std::cout << "    C -> AI side cannon.\n";
    std::cout << "    R -> AI side rook.\n";
    std::cout << "    N -> AI side knight.\n";
    std::cout << "    B -> AI side bishop.\n";
    std::cout << "    A -> AI side advisor.\n";
    std::cout << "    G -> AI side general.\n";
    std::cout << "    p -> our pawn.\n";
    std::cout << "    c -> our cannon.\n";
    std::cout << "    r -> our rook.\n";
    std::cout << "    n -> our knight.\n";
    std::cout << "    b -> our bishop.\n";
    std::cout << "    a -> our advisor.\n";
    std::cout << "    g -> our general.\n";
    std::cout << "    . -> no piece here.\n";
    std::cout << "=======================================\n";
    std::cout << "Press any key to continue.\n";

    // ignore any input.	
    std::string line;
    std::getline(std::cin, line);
}

void state_help(ChessBoard const& cb) {
    print_help_page();
    draw_board(cb);
}

void state_undo(ChessBoard& cb) {
    cb.undo();
    cb.undo();
    draw_board(cb);
}

void state_remake(ChessBoard& cb) {
    cb.clear();
    std::cout << "New cnchess started.\n";
    draw_board(cb);
}

void state_diff(uint16_t& searchDepth) {
    std::string input;
    std::cout << "Current difficulty is " << searchDepth << ", want to change it(y/n)? ";
    
    std::getline(std::cin, input);
    if (input == "n") {
        return;
    }

    std::cout << "change AI difficulty to (1 ~ 5): ";
    std::getline(std::cin, input);
    
    searchDepth = std::stoi(input) % 6;
    if (searchDepth == 0) {
        searchDepth = 1;
    }

    std::cout << "current search depth is " << searchDepth << ".\n";
}

void state_advice(ChessBoard& cb, PieceSide userSide, uint16_t searchDepth) {
    MoveNode advice = gen_best_move(cb, userSide, searchDepth);
    std::string adviceStr = convert_move_to_str(advice);
    std::cout << "Maybe you can try: " << adviceStr 
                        << ", piece is " << piece_get_char(cb.get(advice.beginRow, advice.beginCol))
                        << ".\n";
}

void state_try_move(ChessBoard& cb, std::string const& userInput, PieceSide userSide, PieceSide aiSide, uint16_t searchDepth, bool& running) {
    if (!check_input_is_a_move(userInput)) {
        std::cout << "Input is not a valid move nor instruction, please re-enter(try help ?).\n";
        return;
    }
    
    MoveNode userMove = convert_input_to_move(userInput);
    if (!check_is_this_your_piece(cb, userMove, userSide)){
        std::cout << "This piece is not yours, please choose your piece.\n";
        return;
    }

    if (!check_rule(cb, userMove)){
        std::cout << "Given move doesn't fit for rules, please re-enter.\n";
        return;
    }

    cb.move(userMove);
    draw_board(cb);

    if (check_winner(cb) == userSide){
        std::cout << "Congratulations! You win!\n";
        running = false;
        return;
    }

    std::cout << "AI thinking...\n";

    MoveNode aiMove = gen_best_move(cb, aiSide, searchDepth);
    std::string aiMoveStr = convert_move_to_str(aiMove);
    cb.move(aiMove);
    draw_board(cb);
    std::cout << "AI move: " << aiMoveStr
                << ", piece is '" << piece_get_char(cb.get(aiMove.endRow, aiMove.endCol)) 
                << "'.\n";

    if (check_winner(cb) == aiSide){
        std::cout << "Game over! You lose!\n";
        running = false;
        return;
    }
}

void welcome() {
    std::cout << "Welcome to this cnchess game, down side is you, upper is AI.\n";
    std::cout << "You can type 'help' for more detail or just type 'h2e2' to begin.\n";
}

int main(){
    PieceSide userSide = PS_DOWN;
    PieceSide aiSide = PS_UP;

    ChessBoard cb;
    std::string userInput;
    uint16_t searchDepth = DEFAULT_AI_SEARCH_DEPTH;
    bool running = true;

    welcome();
    draw_board(cb);

    while (running) {
        std::cout << "Your move: ";
        std::getline(std::cin, userInput);

        if (userInput == "help") {
            state_help(cb);
        }
        else if (userInput == "undo") {
            state_undo(cb);
        }
        else if (userInput == "quit") {
            return 0;
        }
        else if (userInput == "exit") {
            return 0;
        }
        else if (userInput == "remake") {
            state_remake(cb);
        }
        else if (userInput == "diff") {
            state_diff(searchDepth);
        }
        else if (userInput == "advice") {
            state_advice(cb, userSide, searchDepth);
        }
        else{
            state_try_move(cb, userInput, userSide, aiSide, searchDepth, running);
        }
    }

    return 0;
}
