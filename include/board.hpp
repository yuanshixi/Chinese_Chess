#pragma once

#include <array>
#include <deque>
#include "piece.hpp"

namespace cnchess {
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
        std::array<std::array<Piece, BOARD_COL_LEN>, BOARD_ROW_LEN> data;
        std::deque<HistoryNode> history;
    public:
        ChessBoard(){
            for (size_t r = 0; r < BOARD_ROW_LEN; ++r) {
                for (size_t c = 0; c < BOARD_COL_LEN; ++c) {
                    data[r][c] = CHESS_BOARD_DEFAULT_TEMPLATE[r][c];
                }
            }
        }

        Piece get(int32_t r, int32_t c) const noexcept {
            return data[r][c];
        }

        void set(int32_t r, int32_t c, Piece p) noexcept {
            data[r][c] = p;
        }

        void move(const MoveNode& moveNode);
        void undo();
    };
};
