#include "../include/moves_gen.hpp"
#include <algorithm>

namespace cnchess {
    static void check_possible_move_and_insert(const ChessBoard& cb, 
                                                PossibleMoves& pm, 
                                                int32_t beginRow, 
                                                int32_t beginCol, 
                                                int32_t endRow, 
                                                int32_t endCol){
        Piece beginP = cb.get(beginRow, beginCol);
        Piece endP = cb.get(endRow, endCol);

        if (endP != P_EO && piece_get_side(beginP) != piece_get_side(endP)){   // not out of chess board, and not the same side.
            pm.emplace_back(beginRow, beginCol, endRow, endCol);
        }
    }

    static void 
    gen_possible_moves_for_pawn(const ChessBoard& cb, PossibleMoves pm, int32_t r, int32_t c, PieceSide side){
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

    static void 
    gen_possible_moves_for_cannon_one_direction(const ChessBoard& cb, 
                                                PossibleMoves& pm, 
                                                int32_t r, 
                                                int32_t c, 
                                                int32_t rGap, 
                                                int32_t cGap, 
                                                PieceSide side){
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

    static void gen_possible_moves_for_cannon(const ChessBoard& cb, PossibleMoves& pm, int32_t r, int32_t c, PieceSide side){
        // go up, down, left, right.
        gen_possible_moves_for_cannon_one_direction(cb, pm, r, c, -1, 0, side);
        gen_possible_moves_for_cannon_one_direction(cb, pm, r, c, +1, 0, side);
        gen_possible_moves_for_cannon_one_direction(cb, pm, r, c, 0, -1, side);
        gen_possible_moves_for_cannon_one_direction(cb, pm, r, c, 0, +1, side);
    }

    static void gen_possible_moves_for_rook_one_direction(const ChessBoard& cb, 
                                                            PossibleMoves& pm, 
                                                            int32_t r, 
                                                            int32_t c, 
                                                            int32_t rGap, 
                                                            int32_t cGap, 
                                                            PieceSide side){
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

    static void gen_possible_moves_for_rook(const ChessBoard& cb, PossibleMoves& pm, int32_t r, int32_t c, PieceSide side){
        // go up, down, left, right.
        gen_possible_moves_for_rook_one_direction(cb, pm, r, c, -1, 0, side);
        gen_possible_moves_for_rook_one_direction(cb, pm, r, c, +1, 0, side);
        gen_possible_moves_for_rook_one_direction(cb, pm, r, c, 0, -1, side);
        gen_possible_moves_for_rook_one_direction(cb, pm, r, c, 0, +1, side);
    }

    static void gen_possible_moves_for_knight(const ChessBoard& cb, PossibleMoves& pm, int32_t r, int32_t c, PieceSide side){
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

    static void gen_possible_moves_for_bishop(const ChessBoard& cb, PossibleMoves& pm, int32_t r, int32_t c, PieceSide side){
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

    static void gen_possible_moves_for_advisor(const ChessBoard& cb, PossibleMoves& pm, int32_t r, int32_t c, PieceSide side){
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

    static void gen_possible_moves_for_general(const ChessBoard& cb, PossibleMoves& pm, int32_t r, int32_t c, PieceSide side){
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
            for (row = r + 1; row < BOARD_ACTUAL_ROW_BEGIN + BOARD_ACTUAL_ROW_LEN ;++row){
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

    // generate possible moves for one side. 
    static PossibleMoves gen_possible_moves(const ChessBoard& cb, PieceSide side){
        PossibleMoves pm;
        pm.reserve(MAX_ONE_SIDE_POSSIBLE_MOVES_LEN);

        int32_t endRow = BOARD_ACTUAL_ROW_BEGIN + BOARD_ACTUAL_ROW_LEN;
        int32_t endCol = BOARD_ACTUAL_COL_BEGIN + BOARD_ACTUAL_COL_LEN;

        Piece p;
        for (int32_t r = BOARD_ACTUAL_ROW_BEGIN; r < endRow; ++r) {
            for (int32_t c = BOARD_ACTUAL_COL_BEGIN; c < endCol; ++c){
                p = cb.get(r, c);

                if (piece_get_side(p) == side){
                    switch (piece_get_type(p))
                    {
                    case PT_PAWN:
                        gen_possible_moves_for_pawn(cb, pm, r, c, side);
                        break;
                    case PT_CANNON:
                        gen_possible_moves_for_cannon(cb, pm, r, c, side);
                        break;
                    case PT_ROOK:
                        gen_possible_moves_for_rook(cb, pm, r, c, side);
                        break;
                    case PT_KNIGHT:
                        gen_possible_moves_for_knight(cb, pm, r, c, side);
                        break;
                    case PT_BISHOP:
                        gen_possible_moves_for_bishop(cb, pm, r, c, side);
                        break;
                    case PT_ADVISOR:
                        gen_possible_moves_for_advisor(cb, pm, r, c, side);
                        break;
                    case PT_GENERAL:
                        gen_possible_moves_for_general(cb, pm, r, c, side);
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

    PossibleMoves gen_possible_moves_upper_side(const ChessBoard& cb) {
        return gen_possible_moves(cb, PieceSide::PS_UP);
    }

    PossibleMoves gen_possible_moves_down_side(const ChessBoard& cb) {
        return gen_possible_moves(cb, PieceSide::PS_DOWN);
    }

    // given move is fit for rule ? return false if not.
    bool check_rule(const ChessBoard& cb, const MoveNode& moveNode){
        Piece p = cb.get(moveNode.beginRow, moveNode.beginCol);
        PossibleMoves pm = gen_possible_moves(cb, piece_get_side(p));

        return std::find(pm.cbegin(), pm.cend(), moveNode) != pm.cend();
    }
};
