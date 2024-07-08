#include "../include/extra_funcs.hpp"

namespace cnchess {
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
};
