#include <limits>
#include "../include/ai.hpp"
#include "../include/moves_gen.hpp"

namespace cnchess {
    /* 
        calculate a chess board's score. 
        upper side value is negative, down side is positive.
    */
    int32_t board_calc_score(const ChessBoard& cb){
        int32_t totalScore = 0;
        int32_t endRow = BOARD_ACTUAL_ROW_BEGIN + BOARD_ACTUAL_ROW_LEN;
        int32_t endCol = BOARD_ACTUAL_COL_BEGIN + BOARD_ACTUAL_COL_LEN;

        Piece p;
        for (int32_t r = BOARD_ACTUAL_ROW_BEGIN; r < endRow; ++r) {
            for (int32_t c = BOARD_ACTUAL_COL_BEGIN; c < endCol; ++c){
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
    static int32_t min_max(ChessBoard& cb, uint8_t searchDepth, int32_t alpha, int32_t beta, PieceSide side){
        if (searchDepth == 0){
            return board_calc_score(cb);
        }

        int32_t minMaxValue;
        if (side == PS_UP){
            int32_t minValue = std::numeric_limits<int32_t>::max();
            PossibleMoves possibleMoves = gen_possible_moves_upper_side(cb);

            for (const MoveNode& node : possibleMoves) {
                cb.move(node);
                minMaxValue = min_max(cb, searchDepth - 1, alpha, beta, PS_DOWN);
                minValue = std::min(minValue, minMaxValue);
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
            PossibleMoves possibleMoves = gen_possible_moves_down_side(cb);

            for (const MoveNode& node : possibleMoves) {
                cb.move(node);
                minMaxValue = min_max(cb, searchDepth - 1, alpha, beta, PS_UP);
                maxValue = std::max(maxValue, minMaxValue);
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
    MoveNode gen_best_move(ChessBoard& cb, PieceSide side, uint8_t searchDepth) {
        int32_t value;
        int32_t alpha = std::numeric_limits<int32_t>::min();
        int32_t beta = std::numeric_limits<int32_t>::max();

        MoveNode bestMove;

        if (side == PS_UP){
            int32_t minValue = beta;
            PossibleMoves possibleMoves = gen_possible_moves_upper_side(cb);

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
            PossibleMoves possibleMoves = gen_possible_moves_down_side(cb);

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
};