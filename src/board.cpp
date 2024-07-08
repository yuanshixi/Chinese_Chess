#include "../include/board.hpp"

namespace cnchess {
    void ChessBoard::move(const MoveNode& moveNode){
        Piece beginPiece = get(moveNode.beginRow, moveNode.beginCol);
        Piece endPiece = get(moveNode.endRow, moveNode.endCol);

        // record the history.
        history.emplace_back(moveNode, beginPiece, endPiece);

        // move the pieces.
        set(moveNode.beginRow, moveNode.beginCol, P_EE);
        set(moveNode.endRow, moveNode.endCol, beginPiece);
    }

    void ChessBoard::undo(){
        if (!history.empty()){   // if history is not empty, reset pieces and pop back.
            const HistoryNode& node = history.back();

            set(node.move.beginRow, node.move.beginCol, node.beginPiece);
            set(node.move.endRow, node.move.endCol, node.endPiece);

            history.pop_back();
        }
    }
};
