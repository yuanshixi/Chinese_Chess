#pragma once

#include <vector>
#include "board.hpp"

namespace cnchess {
    using PossibleMoves = std::vector<MoveNode>;

    PossibleMoves gen_possible_moves_upper_side(const ChessBoard& cb);
    PossibleMoves gen_possible_moves_down_side(const ChessBoard& cb);
    bool check_rule(const ChessBoard& cb, const MoveNode& moveNode);
};
