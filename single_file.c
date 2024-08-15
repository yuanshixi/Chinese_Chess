#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#endif

void log_error_die(const char* fmt, ...) {
    va_list args;

    fprintf(stderr, "[%s %s] ", __DATE__, __TIME__);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    exit(EXIT_FAILURE);
}

/*************************************** piece. ******************************************/
typedef enum PieceSide {
    PS_Up,
    PS_Down,
    PS_Extra
} PieceSide;

typedef enum PieceType {
    PT_Pawn,
    PT_Cannon,
    PT_Rook,
    PT_Knight,
    PT_Bishop,
    PT_Advisor,
    PT_General,
    PT_Empty,
    PT_Out
} PieceType;

typedef enum Piece {
    P_UP,
    P_UC,
    P_UR,
    P_UN,
    P_UB,
    P_UA,
    P_UG,
    P_DP,
    P_DC,
    P_DR,
    P_DN,
    P_DB,
    P_DA,
    P_DG,
    P_EE,
    P_EO
} Piece;

static const char piece_char_mapping[] = {
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

static const PieceSide piece_side_mapping[] = {
    PS_Up, PS_Up, PS_Up, PS_Up, PS_Up, PS_Up, PS_Up, 
    PS_Down, PS_Down, PS_Down, PS_Down, PS_Down, PS_Down, PS_Down, 
    PS_Extra, PS_Extra
};

static const PieceType piece_type_mapping[] = {
    PT_Pawn, PT_Cannon, PT_Rook, PT_Knight, PT_Bishop, PT_Advisor, PT_General, 
    PT_Pawn, PT_Cannon, PT_Rook, PT_Knight, PT_Bishop, PT_Advisor, PT_General, 
    PT_Empty, PT_Out
};

static const PieceSide piece_side_reverse_mapping[] = {
    PS_Down,
    PS_Up,
    PS_Extra
};

char piece_get_char(Piece p) {
    return piece_char_mapping[p];
}

PieceSide piece_get_side(Piece p) {
    return piece_side_mapping[p];
}

PieceType piece_get_type(Piece p) {
    return piece_type_mapping[p];
}

PieceSide piece_side_reverse(PieceSide s) {
    return piece_side_reverse_mapping[s];
}

/*************************************** board. ***************************************/
#define BOARD_ROW_LEN 10
#define BOARD_COL_LEN  9
#define BOARD_ACTUAL_ROW_LEN 14
#define BOARD_ACTUAL_COL_LEN 13

#define BOARD_ACTUAL_ROW_BEGIN 2
#define BOARD_ACTUAL_COL_BEGIN 2
#define BOARD_ACTUAL_ROW_END   (BOARD_ACTUAL_ROW_BEGIN + BOARD_ROW_LEN - 1)
#define BOARD_ACTUAL_COL_END   (BOARD_ACTUAL_COL_BEGIN + BOARD_COL_LEN - 1)

#define BOARD_RIVER_UP    (BOARD_ACTUAL_ROW_BEGIN + 4)
#define BOARD_RIVER_DOWN  (BOARD_ACTUAL_ROW_BEGIN + 5)
#define BOARD_CHU_HAN_BOUNDARY   (BOARD_ACTUAL_ROW_BEGIN + (BOARD_ROW_LEN / 2))

#define BOARD_9_PALACE_UP_TOP     (BOARD_ACTUAL_ROW_BEGIN)
#define BOARD_9_PALACE_UP_BOTTOM  (BOARD_ACTUAL_ROW_BEGIN + 2)
#define BOARD_9_PALACE_UP_LEFT    (BOARD_ACTUAL_COL_BEGIN + 3)
#define BOARD_9_PALACE_UP_RIGHT   (BOARD_ACTUAL_COL_BEGIN + 5)

#define BOARD_9_PALACE_DOWN_TOP     (BOARD_ACTUAL_ROW_BEGIN + 7)
#define BOARD_9_PALACE_DOWN_BOTTOM  (BOARD_ACTUAL_ROW_BEGIN + 9)
#define BOARD_9_PALACE_DOWN_LEFT    (BOARD_ACTUAL_COL_BEGIN + 3)
#define BOARD_9_PALACE_DOWN_RIGHT   (BOARD_ACTUAL_COL_BEGIN + 5)

#define CNCHESS_HISTORY_PRE_ALLOC_CAPACITY   128
#define CNCHESS_MOVES_PRE_ALLOC_CAPACITY   256

typedef struct MoveNode {
    int beginRow, beginCol, endRow, endCol;
} MoveNode;

typedef struct HistoryNode {
    int beginRow, beginCol, endRow, endCol;
    Piece beginP, endP;
} HistoryNode;

typedef struct History {
    HistoryNode* data;
    int length;
    int capacity;
} History;

History* history_create_new(void) {
    History* hist = (History*)malloc(sizeof(History));
    if (hist == NULL) {
        log_error_die("can't create history: memory is not enough.\n");
    }

    hist->capacity = CNCHESS_HISTORY_PRE_ALLOC_CAPACITY;
    hist->length = 0;

    hist->data = (HistoryNode*)malloc(hist->capacity * sizeof(HistoryNode));
    if (hist->data == NULL) {
        log_error_die("can't allocate history data: memory is not enough.\n");
    }

    return hist;
}

void history_destroy(History* hist) {
    if (hist != NULL) {
        free(hist->data);
        free(hist);
    }
}

void history_save(History* hist, MoveNode* mv, Piece beginP, Piece endP) {
    if (hist->length == hist->capacity) {
        int newCapacity = 2 * hist->capacity;
        HistoryNode* temp = (HistoryNode*)realloc(hist->data, newCapacity * sizeof(HistoryNode));
        if (temp == NULL) {
            log_error_die("can't expand history capacity: memory is not enough.\n");
        }

        hist->data = temp;
        hist->capacity = newCapacity;
    }

    HistoryNode* h = &(hist->data[hist->length]);
    h->beginRow = mv->beginRow;
    h->beginCol = mv->beginCol;
    h->endRow = mv->endRow;
    h->endCol = mv->endCol;
    h->beginP = beginP;
    h->endP = endP;

    hist->length += 1;
}

int history_is_empty(History* hist) {
    return hist->length == 0;
}

HistoryNode* history_top(History* hist) {
    return &(hist->data[hist->length - 1]);
}

void history_pop(History* hist) {
    hist->length -= 1;
}

void history_clear(History* hist) {
    hist->length = 0;
}

typedef struct ChessBoard {
    Piece data[BOARD_ACTUAL_ROW_LEN][BOARD_ACTUAL_COL_LEN];
    History* history;
} ChessBoard;

void board_clear(ChessBoard* cb) {
    static const Piece default_board[BOARD_ACTUAL_ROW_LEN][BOARD_ACTUAL_COL_LEN] = {
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

    history_clear(cb->history);
    memcpy(cb->data, default_board, BOARD_ACTUAL_ROW_LEN * BOARD_ACTUAL_COL_LEN * sizeof(Piece));
}

ChessBoard* board_create_new(void) {
    ChessBoard* cb = (ChessBoard*)malloc(sizeof(ChessBoard));
    if (cb == NULL) {
        log_error_die("can't create a chess board: memory is not enough.\n");
    }

    cb->history = history_create_new();
    board_clear(cb);
    return cb;
}

void board_destroy(ChessBoard* cb) {
    if (cb != NULL) {
        history_destroy(cb->history);
        free(cb);
    }
}

void board_move(ChessBoard* cb, MoveNode* mv) {
    Piece beginP = cb->data[mv->beginRow][mv->beginCol];
    Piece endP = cb->data[mv->endRow][mv->endCol];

    history_save(cb->history, mv, beginP, endP);
    cb->data[mv->beginRow][mv->beginCol] = P_EE;
    cb->data[mv->endRow][mv->endCol] = beginP;
}

void board_undo(ChessBoard* cb) {
    if (history_is_empty(cb->history)) {
        return;
    }

    HistoryNode* top = history_top(cb->history);
    cb->data[top->beginRow][top->beginCol] = top->beginP;
    cb->data[top->endRow][top->endCol] = top->endP;
    history_pop(cb->history);
}

/************************************ moves generator. **************************************/
typedef struct Moves {
    MoveNode* data;
    int length;
    int capacity;
} Moves;

Moves* moves_create_new(void) {
    Moves* moves = (Moves*)malloc(sizeof(Moves));
    if (moves == NULL) {
        log_error_die("can't create moves: memory is not enough.\n");
    }

    moves->capacity = CNCHESS_MOVES_PRE_ALLOC_CAPACITY;
    moves->length = 0;

    moves->data = (MoveNode*)malloc(moves->capacity * sizeof(MoveNode));
    if (moves->data == NULL) {
        log_error_die("can't allocate moves data: memory is not enough.\n");
    }

    return moves;
}

void moves_destroy(Moves* moves) {
    if (moves != NULL) {
        free(moves->data);
        free(moves);
    }
}

void moves_add(Moves* moves, int beginRow, int beginCol, int endRow, int endCol) {
    if (moves->length == moves->capacity) {
        int newCapacity = 2 * moves->capacity;
        MoveNode* temp = (MoveNode*)realloc(moves->data, newCapacity * sizeof(MoveNode));
        if (temp == NULL) {
            log_error_die("can't expand moves capacity: memory is not enough.\n");
        }

        moves->data = temp;
        moves->capacity = newCapacity;
    }

    MoveNode* m = &(moves->data[moves->length]);
    m->beginRow = beginRow;
    m->beginCol = beginCol;
    m->endRow = endRow;
    m->endCol = endCol;

    moves->length += 1;
}

void insert_move_with_check(ChessBoard* cb, Moves* moves, int beginRow, int beginCol, int endRow, int endCol) {
    Piece beginP = cb->data[beginRow][beginCol];
    Piece endP = cb->data[endRow][endCol];

    if (endP != P_EO && piece_get_side(beginP) != piece_get_side(endP)) {
        moves_add(moves, beginRow, beginCol, endRow, endCol);
    }
}

void gen_moves_pawn(ChessBoard* cb, Moves* moves, int r, int c, PieceSide s) {
    if (s == PS_Up) {
        insert_move_with_check(cb, moves, r, c, r + 1, c);

        if (r > BOARD_RIVER_UP){    /* cross the river ? */
            insert_move_with_check(cb, moves, r, c, r, c - 1);
            insert_move_with_check(cb, moves, r, c, r, c + 1);
        }
    }
    else if (s == PS_Down) {
        insert_move_with_check(cb, moves, r, c, r - 1, c);

        if (r < BOARD_RIVER_DOWN){
            insert_move_with_check(cb, moves, r, c, r, c - 1);
            insert_move_with_check(cb, moves, r, c, r, c + 1);
        }
    }
}

void gen_moves_cannon_one_direction(ChessBoard* cb, Moves* moves, int r, int c, int rGap, int cGap, PieceSide s) {
    int row, col;
    Piece p;

    for (row = r + rGap, col = c + cGap; ;row += rGap, col += cGap){
        p = cb->data[row][col];

        if (p == P_EE){    /* empty piece, then insert it. */
            moves_add(moves, r, c, row, col);
        }
        else {   /* upper piece, down piece or out of chess board, break immediately. */
            break;
        }
    }

    if (p != P_EO){   /* not out of chess board, check if we can add an enemy piece. */
        for (row = row + rGap, col = col + cGap; ;row += rGap, col += cGap){
            p = cb->data[row][col];
        
            if (p == P_EE){    /* empty, then continue search. */
                continue;
            }
            else if (piece_get_side(p) == piece_side_reverse(s)){   /* enemy piece, then insert it and break. */
                moves_add(moves, r, c, row, col);
                break;
            }
            else {    /* self side piece or out of chess board, break. */
                break;
            }
        }
    }
}

void gen_moves_cannon(ChessBoard* cb, Moves* moves, int r, int c, PieceSide s) {
    /* go up, down, left, right. */
    gen_moves_cannon_one_direction(cb, moves, r, c, -1, 0, s);
    gen_moves_cannon_one_direction(cb, moves, r, c, +1, 0, s);
    gen_moves_cannon_one_direction(cb, moves, r, c, 0, -1, s);
    gen_moves_cannon_one_direction(cb, moves, r, c, 0, +1, s);
}

void gen_moves_rook_one_direction(ChessBoard* cb, Moves* moves, int r, int c, int rGap, int cGap, PieceSide s) {
    int row, col;
    Piece p;

    for (row = r + rGap, col = c + cGap; ;row += rGap, col += cGap){
        p = cb->data[row][col];

        if (p == P_EE){    /* empty piece, then insert it. */
            moves_add(moves, r, c, row, col);
        }
        else {   /* upper piece, down piece or out of chess board, break immediately. */
            break;
        }
    }

    if (piece_get_side(p) == piece_side_reverse(s)){   /* enemy piece, then insert it. */
        moves_add(moves, r, c, row, col);
    }
}

void gen_moves_rook(ChessBoard* cb, Moves* moves, int r, int c, PieceSide s) {
    /* go up, down, left, right. */
    gen_moves_rook_one_direction(cb, moves, r, c, -1, 0, s);
    gen_moves_rook_one_direction(cb, moves, r, c, +1, 0, s);
    gen_moves_rook_one_direction(cb, moves, r, c, 0, -1, s);
    gen_moves_rook_one_direction(cb, moves, r, c, 0, +1, s);
}

void gen_moves_knight(ChessBoard* cb, Moves* moves, int r, int c, PieceSide s /* unused. */) {
    Piece p;

    if ((p = cb->data[r + 1][c]) == P_EE){    /* if not lame horse leg ? */
        insert_move_with_check(cb, moves, r, c, r + 2, c + 1);
        insert_move_with_check(cb, moves, r, c, r + 2, c - 1);
    }

    if ((p = cb->data[r - 1][c]) == P_EE){
        insert_move_with_check(cb, moves, r, c, r - 2, c + 1);
        insert_move_with_check(cb, moves, r, c, r - 2, c - 1);
    }

    if ((p = cb->data[r][c + 1]) == P_EE){
        insert_move_with_check(cb, moves, r, c, r + 1, c + 2);
        insert_move_with_check(cb, moves, r, c, r - 1, c + 2);
    }

    if ((p = cb->data[r][c - 1]) == P_EE){
        insert_move_with_check(cb, moves, r, c, r + 1, c - 2);
        insert_move_with_check(cb, moves, r, c, r - 1, c - 2);
    }
}

void gen_moves_bishop_up(ChessBoard* cb, Moves* moves, int r, int c) {
    Piece p;

    if (r + 2 <= BOARD_RIVER_UP){       /* bishop can't cross river. */
        if ((p = cb->data[r + 1][c + 1]) == P_EE){    /* bishop can move only if Xiang Yan is empty. */
            insert_move_with_check(cb, moves, r, c, r + 2, c + 2);
        }

        if ((p = cb->data[r + 1][c - 1]) == P_EE){
            insert_move_with_check(cb, moves, r, c, r + 2, c - 2);
        }
    }

    if ((p = cb->data[r - 1][c + 1]) == P_EE){
        insert_move_with_check(cb, moves, r, c, r - 2, c + 2);
    }

    if ((p = cb->data[r - 1][c - 1]) == P_EE){
        insert_move_with_check(cb, moves, r, c, r - 2, c - 2);
    }
}

void gen_moves_bishop_down(ChessBoard* cb, Moves* moves, int r, int c) {
    Piece p;

    if (r - 2 >= BOARD_RIVER_DOWN){
        if ((p = cb->data[r - 1][c + 1]) == P_EE){
            insert_move_with_check(cb, moves, r, c, r - 2, c + 2);
        }

        if ((p = cb->data[r - 1][c - 1]) == P_EE){
            insert_move_with_check(cb, moves, r, c, r - 2, c - 2);
        }
    }

    if ((p = cb->data[r + 1][c + 1]) == P_EE){
        insert_move_with_check(cb, moves, r, c, r + 2, c + 2);
    }

    if ((p = cb->data[r + 1][c - 1]) == P_EE){
        insert_move_with_check(cb, moves, r, c, r + 2, c - 2);
    }
}

void gen_moves_bishop(ChessBoard* cb, Moves* moves, int r, int c, PieceSide s) {
    if (s == PS_Up) {
        gen_moves_bishop_up(cb, moves, r, c);
    }
    else if (s == PS_Down) {
        gen_moves_bishop_down(cb, moves, r, c);
    }
}

void gen_moves_advisor_up(ChessBoard* cb, Moves* moves, int r, int c) {
    if (r + 1 <= BOARD_9_PALACE_UP_BOTTOM && c + 1 <= BOARD_9_PALACE_UP_RIGHT) {   /* walk diagonal lines. */
        insert_move_with_check(cb, moves, r, c, r + 1, c + 1);
    }

    if (r + 1 <= BOARD_9_PALACE_UP_BOTTOM && c - 1 >= BOARD_9_PALACE_UP_LEFT) {
        insert_move_with_check(cb, moves, r, c, r + 1, c - 1);
    }

    if (r - 1 >= BOARD_9_PALACE_UP_TOP && c + 1 <= BOARD_9_PALACE_UP_RIGHT) {
        insert_move_with_check(cb, moves, r, c, r - 1, c + 1);
    }

    if (r - 1 >= BOARD_9_PALACE_UP_TOP && c - 1 >= BOARD_9_PALACE_UP_LEFT) {
        insert_move_with_check(cb, moves, r, c, r - 1, c - 1);
    }
}

void gen_moves_advisor_down(ChessBoard* cb, Moves* moves, int r, int c) {
    if (r + 1 <= BOARD_9_PALACE_DOWN_BOTTOM && c + 1 <= BOARD_9_PALACE_DOWN_RIGHT) {
        insert_move_with_check(cb, moves, r, c, r + 1, c + 1);
    }

    if (r + 1 <= BOARD_9_PALACE_DOWN_BOTTOM && c - 1 >= BOARD_9_PALACE_DOWN_LEFT) {
        insert_move_with_check(cb, moves, r, c, r + 1, c - 1);
    }

    if (r - 1 >= BOARD_9_PALACE_DOWN_TOP && c + 1 <= BOARD_9_PALACE_DOWN_RIGHT) {
        insert_move_with_check(cb, moves, r, c, r - 1, c + 1);
    }

    if (r - 1 >= BOARD_9_PALACE_DOWN_TOP && c - 1 >= BOARD_9_PALACE_DOWN_LEFT) {
        insert_move_with_check(cb, moves, r, c, r - 1, c - 1);
    }
}

void gen_moves_advisor(ChessBoard* cb, Moves* moves, int r, int c, PieceSide s) {
    if (s == PS_Up) {
        gen_moves_advisor_up(cb, moves, r, c);
    }
    else if (s == PS_Down) {
        gen_moves_advisor_down(cb, moves, r, c);
    }
}

void gen_moves_general_up(ChessBoard* cb, Moves* moves, int r, int c) {
    int row;
    Piece p;

    if (r + 1 <= BOARD_9_PALACE_UP_BOTTOM){   /* walk horizontal or vertical. */
        insert_move_with_check(cb, moves, r, c, r + 1, c);
    }

    if (r - 1 >= BOARD_9_PALACE_UP_TOP){
        insert_move_with_check(cb, moves, r, c, r - 1, c);
    }

    if (c + 1 <= BOARD_9_PALACE_UP_RIGHT){
        insert_move_with_check(cb, moves, r, c, r, c + 1);
    }

    if (c - 1 >= BOARD_9_PALACE_UP_LEFT){
        insert_move_with_check(cb, moves, r, c, r, c - 1);
    }

    /* check if both generals faced each other directly. */
    for (row = r + 1; row <= BOARD_ACTUAL_ROW_END;++row){
        p = cb->data[row][c];

        if (p == P_EE){
            continue;
        }
        else if (p == P_DG){
            moves_add(moves, r, c, row, c);
            break;
        }
        else {
            break;
        }
    }
}

void gen_moves_general_down(ChessBoard* cb, Moves* moves, int r, int c) {
    int row;
    Piece p;

    if (r + 1 <= BOARD_9_PALACE_DOWN_BOTTOM){
        insert_move_with_check(cb, moves, r, c, r + 1, c);
    }

    if (r - 1 >= BOARD_9_PALACE_DOWN_TOP){
        insert_move_with_check(cb, moves, r, c, r - 1, c);
    }

    if (c + 1 <= BOARD_9_PALACE_DOWN_RIGHT){
        insert_move_with_check(cb, moves, r, c, r, c + 1);
    }

    if (c - 1 >= BOARD_9_PALACE_DOWN_LEFT){
        insert_move_with_check(cb, moves, r, c, r, c - 1);
    }

    for (row = r - 1; row >= BOARD_ACTUAL_ROW_BEGIN ;--row){
        p = cb->data[row][c];

        if (p == P_EE){
            continue;
        }
        else if (p == P_UG){
            moves_add(moves, r, c, row, c);
            break;
        }
        else {
            break;
        }
    }
}

void gen_moves_general(ChessBoard* cb, Moves* moves, int r, int c, PieceSide s) {
    if (s == PS_Up) {
        gen_moves_general_up(cb, moves, r, c);
    }
    else if (s == PS_Down) {
        gen_moves_general_down(cb, moves, r, c);
    }
}

Moves* gen_moves(ChessBoard* cb, PieceSide s) {
    if (s == PS_Extra) {
        return NULL;
    }

    Piece p;
    int r, c;

    Moves* moves = moves_create_new();
    for (r = BOARD_ACTUAL_ROW_BEGIN; r <= BOARD_ACTUAL_ROW_END; ++r) {
        for (c = BOARD_ACTUAL_COL_BEGIN; c <= BOARD_ACTUAL_COL_END; ++c) {
            p = cb->data[r][c];

            if (piece_get_side(p) == s) {
                switch(piece_get_type(p)) {
                    case PT_Pawn:
                        gen_moves_pawn(cb, moves, r, c, s);
                        break;
                    case PT_Cannon:
                        gen_moves_cannon(cb, moves, r, c, s);
                        break;
                    case PT_Rook:
                        gen_moves_rook(cb, moves, r, c, s);
                        break;
                    case PT_Knight:
                        gen_moves_knight(cb, moves, r, c, s);
                        break;
                    case PT_Bishop:
                        gen_moves_bishop(cb, moves, r, c, s);
                        break;
                    case PT_Advisor:
                        gen_moves_advisor(cb, moves, r, c, s);
                        break;
                    case PT_General:
                        gen_moves_general(cb, moves, r, c, s);
                        break;
                    case PT_Empty:
                    case PT_Out:
                    default:
                        break;
                }
            }
        }
    }

    return moves;
}

/**************************************** evaluate. ****************************************/
static const long piece_value_mapping[] = {
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

static const long piece_pos_value_mapping[][BOARD_ROW_LEN][BOARD_COL_LEN] = {
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

#define piece_get_value(p)  \
    piece_value_mapping[(p)]

#define piece_get_pos_value(p, r, c) \
    piece_pos_value_mapping[(p)][(r)][(c)]

long evaluate_board(ChessBoard* cb) {
    long totalScore = 0;

    Piece p;
    int r, c;
    for (r = BOARD_ACTUAL_ROW_BEGIN; r <= BOARD_ACTUAL_ROW_END; ++r) {
        for (c = BOARD_ACTUAL_COL_BEGIN; c <= BOARD_ACTUAL_COL_END; ++c){
            p = cb->data[r][c];

			if (p != P_EE){
				totalScore += piece_get_value(p);
				totalScore += piece_get_pos_value(p, r - BOARD_ACTUAL_ROW_BEGIN, c - BOARD_ACTUAL_COL_BEGIN);	
            }
        }
    }

    return totalScore;
}

/*************************************** AI. **************************************/
#define BOARD_SCORE_MIN   -1000000
#define BOARD_SCORE_MAX    1000000

long compare_min(long left, long right) {
    return left <= right ? left : right;
}

long compare_max(long left, long right) {
    return left >= right ? left : right;
}

/*
    the smaller, the better for ai, the bigger, the better for user.
    param: PieceSide s can't be PS_Extra.
*/
long min_max(ChessBoard* cb, unsigned int searchDepth, int alpha, int beta, PieceSide s) {
    long minValue, maxValue;
    Moves* moves;
    MoveNode* mv;
    int i;

    if (searchDepth == 0) {
        return evaluate_board(cb);
    }

    if (s == PS_Up) {
        minValue = BOARD_SCORE_MAX;
        moves = gen_moves(cb, s);

        for (i = 0; i < moves->length; ++i) {
            mv = &(moves->data[i]);

            board_move(cb, mv);
            minValue = compare_min(minValue, min_max(cb, searchDepth - 1, alpha, beta, PS_Down));
            board_undo(cb);

            beta = compare_min(beta, minValue);
            if (alpha >= beta) {
                break;
            }
        }

        moves_destroy(moves);
        return minValue;
    }
    else {
        maxValue = BOARD_SCORE_MIN;
        moves = gen_moves(cb, s);

        for (i = 0; i < moves->length; ++i) {
            mv = &(moves->data[i]);

            board_move(cb, mv);
            maxValue = compare_max(maxValue, min_max(cb, searchDepth - 1, alpha, beta, PS_Up));
            board_undo(cb);

            alpha = compare_max(alpha, maxValue);
            if (alpha >= beta) {
                break;
            }
        }

        moves_destroy(moves);
        return maxValue;
    }
}

MoveNode gen_best_move_for(ChessBoard* cb, PieceSide s /* can't be PS_Extra */, unsigned int searchDepth) {
    long value, minValue, maxValue;
    Moves* moves;
    MoveNode* mv;
    MoveNode bestMove;
    int i;

    if (s == PS_Up) {
        minValue = BOARD_SCORE_MAX;
        moves = gen_moves(cb, s);

        for (i = 0; i < moves->length; ++i) {
            mv = &(moves->data[i]);

            board_move(cb, mv);
            value = min_max(cb, searchDepth, BOARD_SCORE_MIN, BOARD_SCORE_MAX, PS_Down);
            board_undo(cb);

            if (value <= minValue) {
                minValue = value;
                memcpy(&bestMove, mv, sizeof(MoveNode));
            }
        }

        moves_destroy(moves);
        return bestMove;
    }
    else {
        maxValue = BOARD_SCORE_MIN;
        moves = gen_moves(cb, s);

        for (i = 0; i < moves->length; ++i) {
            mv = &(moves->data[i]);

            board_move(cb, mv);
            value = min_max(cb, searchDepth, BOARD_SCORE_MIN, BOARD_SCORE_MAX, PS_Up);
            board_undo(cb);

            if (value >= maxValue) {
                maxValue = value;
                memcpy(&bestMove, mv, sizeof(MoveNode));
            }
        }

        moves_destroy(moves);
        return bestMove;
    }
}

/************************************** string. *****************************************/
#define STRING_DEFAULT_CAPACITY   32

typedef struct String {
    char* data;
    int length;
    int capacity;
} String;

String* string_create_new(void) {
    String* str = (String*)malloc(sizeof(String));
    if (str == NULL) {
        log_error_die("can't create string: memory is not enough.\n");
    }

    str->capacity = STRING_DEFAULT_CAPACITY;
    str->length = 0;

    str->data = (char*)malloc(str->capacity * sizeof(char));
    if (str->data == NULL) {
        log_error_die("can't allocate string data: memory is not enough.\n");
    }

    return str;
}

void string_destroy(String* str) {
    if (str != NULL) {
        free(str->data);
        free(str);
    }
}

void string_clear(String* str) {
    str->length = 0;
}

void string_push_char(String* str, char c) {
    if (str->length == str->capacity - 1) {
        int newCapacity = 2 * str->capacity;
        char* temp = (char*)realloc(str->data, newCapacity * sizeof(char));
        if (temp == NULL) {
            log_error_die("can't expand string capacity: memory is not enough.\n");
        }

        str->data = temp;
        str->capacity = newCapacity;
    }

    str->data[str->length] = c;
    str->length += 1;
    str->data[str->length] = '\0';
}

void string_get_line(String* str) {
    char c;

    string_clear(str);
    while (1) {
        c = getchar();

        if (c == '\n' || c == EOF) {
            break;
        }
        else {
            string_push_char(str, c);
        }
    }
}

int string_compare_c_str(String* left, const char* right) {
    return strcmp(left->data, right) == 0;
}

/**************************************** color output. ***************************************/
typedef enum ConsoleColor{
    CC_Black, 
    CC_Red, 
    CC_Green, 
    CC_Yellow, 
    CC_Blue, 
    CC_Magenta, 
    CC_Cyan, 
    CC_White,
    CC_BoldBlack, 
    CC_BoldRed, 
    CC_BoldGreen, 
    CC_BoldYellow, 
    CC_BoldBlue, 
    CC_BoldMagenta, 
    CC_BoldCyan, 
    CC_BoldWhite
} ConsoleColor;

typedef struct ConsoleColorContext {
    #ifdef _WIN32
        HANDLE hOutHandle;
        WORD oldColorAttrs;
    #endif
} ConsoleColorContext;

void console_color_context_init(ConsoleColorContext* ctx) {
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	    ctx->hOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	    GetConsoleScreenBufferInfo(ctx->hOutHandle, &csbiInfo);
	    ctx->oldColorAttrs = csbiInfo.wAttributes;
    #endif
}

#ifdef _WIN32
/* 
    referenced from:
    https://stackoverflow.com/questions/17125440/c-win32-console-color 
*/
WORD get_windows_color_attr(int color) {
    switch(color) {
        case CC_Black: 
            return 0;
        case CC_Blue: 
            return FOREGROUND_BLUE;
        case CC_Green: 
            return FOREGROUND_GREEN;
        case CC_Cyan: 
            return FOREGROUND_GREEN | FOREGROUND_BLUE;
        case CC_Red: 
            return FOREGROUND_RED;
        case CC_Magenta: 
            return FOREGROUND_RED | FOREGROUND_BLUE;
        case CC_Yellow: 
            return FOREGROUND_RED | FOREGROUND_GREEN;
        case CC_White:
            return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        case CC_BoldBlack: 
            return 0 | FOREGROUND_INTENSITY;
        case CC_BoldBlue: 
            return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case CC_BoldGreen: 
            return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        case CC_BoldCyan: 
            return FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case CC_BoldRed: 
            return FOREGROUND_RED | FOREGROUND_INTENSITY;
        case CC_BoldMagenta: 
            return FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case CC_BoldYellow: 
            return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        case CC_BoldWhite:
        default:
            return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    }
};
#endif

void reset_console_color(ConsoleColorContext* ctx) {
    #ifdef __linux__
        printf("\033[0m");
    #elif defined(_WIN32)
        SetConsoleTextAttribute(ctx->hOutHandle, ctx->oldColorAttrs);
    #endif
}

void set_console_color(ConsoleColorContext* ctx, ConsoleColor cc) {
    #ifdef __linux__
    switch(cc) {
        case CC_Black:
            printf("\033[30m"); break;
        case CC_Red:
            printf("\033[31m"); break;
        case CC_Green:
            printf("\033[32m"); break;
        case CC_Yellow:       
            printf("\033[33m"); break;
        case CC_Blue:         
            printf("\033[34m"); break;
        case CC_Magenta:      
            printf("\033[35m"); break;
        case CC_Cyan:         
            printf("\033[36m"); break;
        case CC_White:        
            printf("\033[37m"); break;
        case CC_BoldBlack:    
            printf("\033[1m\033[30m"); break;
        case CC_BoldRed:      
            printf("\033[1m\033[31m"); break;
        case CC_BoldGreen:    
            printf("\033[1m\033[32m"); break;
        case CC_BoldYellow:   
            printf("\033[1m\033[33m"); break;
        case CC_BoldBlue:     
            printf("\033[1m\033[34m"); break;
        case CC_BoldMagenta:  
            printf("\033[1m\033[35m"); break;
        case CC_BoldCyan:     
            printf("\033[1m\033[36m"); break;
        case CC_BoldWhite:  
        default:  
            printf("\033[1m\033[37m"); break;
    }
    #elif defined(_WIN32)
    SetConsoleTextAttribute(ctx->hOutHandle, get_windows_color_attr(cc));
    #endif
}

/************************************ game functions. *************************************/
int check_rule(ChessBoard* cb, MoveNode* mv) {
    int i;
    int valid = 0;
    MoveNode* cursor;
    Piece p = cb->data[mv->beginRow][mv->beginCol];
    Moves* moves = gen_moves(cb, piece_get_side(p));

    for (i = 0; i < moves->length; ++i) {
        cursor = &(moves->data[i]);

        if (memcmp(mv, cursor, sizeof(MoveNode)) == 0) {
            valid = 1;
            break;
        }
    }

    moves_destroy(moves);
    return valid;
}

void draw_board(ConsoleColorContext* ctx, ChessBoard* cb) {
    int n = BOARD_ROW_LEN - 1;

    set_console_color(ctx, CC_BoldWhite);
    printf("\n    +-------------------+\n");

    int r, c;
    Piece p;
    for (r = BOARD_ACTUAL_ROW_BEGIN; r <= BOARD_ACTUAL_ROW_END; ++r) {
        if (r == BOARD_CHU_HAN_BOUNDARY) {
            set_console_color(ctx, CC_White);
            printf("    |-+-+-+-+-+-+-+-+-+-|\n");
            printf("    |-x-x-x-x-x-x-x-x-x-|\n");
        }

        set_console_color(ctx, CC_White);
        printf(" %d  | ", n--);

        for (c = BOARD_ACTUAL_COL_BEGIN; c <= BOARD_ACTUAL_COL_END; ++c){
            p = cb->data[r][c];

            if (piece_get_side(p) == PS_Up) {
                set_console_color(ctx, CC_BoldBlue);
                printf("%c", piece_get_char(p));
                set_console_color(ctx, CC_White);
                printf(" ");
            }
            else if (piece_get_side(p) == PS_Down) {
                set_console_color(ctx, CC_BoldRed);
                printf("%c", piece_get_char(p));
                set_console_color(ctx, CC_White);
                printf(" ");
            }
            else {
                printf("%c ", piece_get_char(p));
            }
        }

        set_console_color(ctx, CC_White);
        printf("|\n");
    }

    set_console_color(ctx, CC_White);
    printf("    +-------------------+\n");
    printf("\n      a b c d e f g h i\n\n");
}

int check_if_string_is_move(String* str) {
    if (str->length < 4) {   /* string like 'b2e2' is a move. */
        return 0;
    }

    return (str->data[0] >= 'a' && str->data[0] <= 'i') &&
            (str->data[1] >= '0' && str->data[1] <= '9') &&
            (str->data[2] >= 'a' && str->data[2] <= 'i') &&
            (str->data[3] >= '0' && str->data[3] <= '9');
}

MoveNode convert_string_to_move(String* str) {
    MoveNode mv;

    mv.beginRow = BOARD_ACTUAL_ROW_BEGIN + 9 - (str->data[1] - '0');
    mv.beginCol = BOARD_ACTUAL_COL_BEGIN + str->data[0] - 'a';
    mv.endRow = BOARD_ACTUAL_ROW_BEGIN + 9 - (str->data[3] - '0');
    mv.endCol = BOARD_ACTUAL_COL_BEGIN + str->data[2] - 'a';

    return mv;
}

String* convert_move_to_string(MoveNode* mv) {
    String* str = string_create_new();

    string_push_char(str, mv->beginCol - BOARD_ACTUAL_COL_BEGIN + 'a');
    string_push_char(str, 9 - (mv->beginRow - BOARD_ACTUAL_ROW_BEGIN) + '0');
    string_push_char(str, mv->endCol - BOARD_ACTUAL_COL_BEGIN + 'a');
    string_push_char(str, 9 - (mv->endRow - BOARD_ACTUAL_ROW_BEGIN) + '0');

    return str;
}

int is_this_your_piece(ChessBoard* cb, MoveNode* mv, PieceSide userSide) {
    Piece p = cb->data[mv->beginRow][mv->beginCol];
    return piece_get_side(p) == userSide;
}

PieceSide check_winner(ChessBoard* cb) {
    int upAlive = 0;
    int downAlive = 0;

    int r, c;
    for (r = BOARD_9_PALACE_UP_TOP; r <= BOARD_9_PALACE_UP_BOTTOM; ++r) {
        for (c = BOARD_9_PALACE_UP_LEFT; c <= BOARD_9_PALACE_UP_RIGHT; ++c) {
            if (cb->data[r][c] == P_UG) {
                upAlive = 1;
                break;
            }
        }
    }

    for (r = BOARD_9_PALACE_DOWN_TOP; r <= BOARD_9_PALACE_DOWN_BOTTOM; ++r) {
        for (c = BOARD_9_PALACE_DOWN_LEFT; c <= BOARD_9_PALACE_DOWN_RIGHT; ++c) {
            if (cb->data[r][c] == P_DG) {
                downAlive = 1;
                break;
            }
        }
    }

    if (upAlive && downAlive) {
        return PS_Extra;
    }
    else if (upAlive) {
        return PS_Up;
    }
    else {
        return PS_Down;
    }
}

void welcome(ConsoleColorContext* ctx, unsigned int difficulty) {
    set_console_color(ctx, CC_White);

    printf("\n%s %s\n\n", __DATE__, __TIME__);
    printf("  Dear Player,\n");
    printf("    Welcome to cnchess game, down side is you. \n");
    printf("    AI difficulty is %d.\n", difficulty);
    printf("    Type 'help' to get more details.\n");
}

void print_help_page(void) {
    printf("\n=======================================\n");
    printf("Help Page\n\n");
    printf("    1. help         - this page.\n");
    printf("    2. b2e2         - input like this will be parsed as a move.\n");
    printf("    3. undo         - undo the previous move.\n");
    printf("    4. exit or quit - exit the game.\n");
    printf("    5. remake       - remake the game.\n");
    printf("    6. diff         - change the difficulty.\n");
    printf("    7. advice       - give me a best move.\n\n");
    printf("  The characters on the board have the following relationships: \n\n");
    printf("    P -> AI side pawn.\n");
    printf("    C -> AI side cannon.\n");
    printf("    R -> AI side rook.\n");
    printf("    N -> AI side knight.\n");
    printf("    B -> AI side bishop.\n");
    printf("    A -> AI side advisor.\n");
    printf("    G -> AI side general.\n");
    printf("    p -> our pawn.\n");
    printf("    c -> our cannon.\n");
    printf("    r -> our rook.\n");
    printf("    n -> our knight.\n");
    printf("    b -> our bishop.\n");
    printf("    a -> our advisor.\n");
    printf("    g -> our general.\n");
    printf("    . -> no piece here.\n");
    printf("=======================================\n");
    printf("Press 'Enter' to continue.\n");

    char c;
    while (1) {
        c = getchar();

        if (c == '\n' || c == EOF) {
            return;
        }
    }
}

void state_help(ConsoleColorContext* ctx, ChessBoard* cb) {
    print_help_page();
    draw_board(ctx, cb);
}

void state_undo(ConsoleColorContext* ctx, ChessBoard* cb) {
    board_undo(cb);
    board_undo(cb);
    draw_board(ctx, cb);
}

void state_remake(ConsoleColorContext* ctx, ChessBoard* cb) {
    board_clear(cb);
    printf("new board is ready.\n");
    draw_board(ctx, cb);
}

void state_diff(unsigned int* difficulty, String* input) {
    printf("Current difficulty is %d, want to change it(y/n)? ", *difficulty);
    string_get_line(input);

    if (string_compare_c_str(input, "n")) {
        return;
    }

    printf("change AI difficulty to (1 ~ 5): ");
    string_get_line(input);
    
    *difficulty = atoi(input->data) % 5;
    if (*difficulty == 0) {
        *difficulty = 1;
    }

    printf("current search depth is %d.\n", *difficulty);
}

void state_advice(ChessBoard* cb, PieceSide userSide, unsigned int difficulty) {
    MoveNode advice = gen_best_move_for(cb, userSide, difficulty);
    Piece p = cb->data[advice.beginRow][advice.beginCol];
    String* adviceStr = convert_move_to_string(&advice);

    printf("Maybe you can try: %s, piece is '%c'\n", adviceStr->data, piece_get_char(p));
    string_destroy(adviceStr);
}

void state_try_move(ConsoleColorContext* ctx, 
                    ChessBoard* cb, 
                    String* input, 
                    PieceSide userSide, 
                    PieceSide aiSide, 
                    unsigned int difficulty, 
                    int* running) {
    if (!check_if_string_is_move(input)) {
        printf("Input is not a valid move nor instruction, please re-enter(try help ?).\n");
        return;
    }
    
    MoveNode userMove = convert_string_to_move(input);
    if (!is_this_your_piece(cb, &userMove, userSide)){
        printf("This piece is not yours, please choose your piece.\n");
        return;
    }

    if (!check_rule(cb, &userMove)){
        printf("Given move doesn't fit for rules, please re-enter.\n");
        return;
    }

    board_move(cb, &userMove);
    draw_board(ctx, cb);

    if (check_winner(cb) == userSide){
        printf("Congratulations! You win!\n");
        *running = 0;
        return;
    }

    printf("AI thinking...\n");

    MoveNode aiMove = gen_best_move_for(cb, aiSide, difficulty);
    Piece p = cb->data[aiMove.beginRow][aiMove.beginCol];
    String* aiMoveStr = convert_move_to_string(&aiMove);

    board_move(cb, &aiMove);
    draw_board(ctx, cb);
    printf("AI move: %s, piece is '%c'\n", aiMoveStr->data, piece_get_char(p));
    string_destroy(aiMoveStr);

    if (check_winner(cb) == aiSide){
        printf("Game over! You lose!\n");
        *running = 0;
        return;
    }
}

int main() {
    ChessBoard* cb = board_create_new();
    String* input = string_create_new();
    ConsoleColorContext ctx;
    int running = 1;
    unsigned int difficulty = 4;
    PieceSide userSide = PS_Down;
    PieceSide aiSide = PS_Up;

    console_color_context_init(&ctx);
    welcome(&ctx, difficulty);
    draw_board(&ctx, cb);
    printf("Your move(like 'b2e2'): ");
    string_get_line(input);

    while (running) {
        if (string_compare_c_str(input, "help")) {
            state_help(&ctx, cb);
        }
        else if (string_compare_c_str(input, "undo")) {
            state_undo(&ctx, cb);
        }
        else if (string_compare_c_str(input, "quit")) {
            running = 0; break;
        }
        else if (string_compare_c_str(input, "exit")) {
            running = 0; break;
        }
        else if (string_compare_c_str(input, "remake")) {
            state_remake(&ctx, cb);
        }
        else if (string_compare_c_str(input, "diff")) {
            state_diff(&difficulty, input);
        }
        else if (string_compare_c_str(input, "advice")) {
            state_advice(cb, userSide, difficulty);
        }
        else {
            state_try_move(&ctx, cb, input, userSide, aiSide, difficulty, &running);
        }

        printf("Your move: ");
        string_get_line(input);
    }

    reset_console_color(&ctx);
    board_destroy(cb);
    string_destroy(input);
    return 0;
}
