#ifdef CNCHESS_CONSOLE_VERSION_ENABLED

#include <iostream>
#include <string>
#include "constants.hpp"
#include "board.hpp"
#include "moves_gen.hpp"
#include "ai.hpp"
#include "extra_funcs.hpp"

using namespace cnchess;

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

void print_board_to_console(const ChessBoard& cb){
    int splitLineIndex = BOARD_ACTUAL_ROW_BEGIN + (BOARD_ACTUAL_ROW_LEN / 2);
    int endRow = BOARD_ACTUAL_ROW_BEGIN + BOARD_ACTUAL_ROW_LEN;
    int endCol = BOARD_ACTUAL_COL_BEGIN + BOARD_ACTUAL_COL_LEN;
    int n = BOARD_ACTUAL_ROW_LEN - 1;

    std::cout << "\n    +-------------------+\n";

    int r, c;
    for (r = BOARD_ACTUAL_ROW_BEGIN; r < endRow; ++r) {
        if (r == splitLineIndex){
            std::cout << "    |===================|\n";
            std::cout << "    |===================|\n";
        }

        std::cout << " " << n-- << "  | ";

        for (c = BOARD_ACTUAL_COL_BEGIN; c < endCol; ++c){
            std::cout << piece_get_char(cb.get(r, c)) << " ";
        }

        std::cout << "|\n";
    }

    std::cout << "    +-------------------+\n";
    std::cout << "\n      a b c d e f g h i\n\n";
}

void print_help_page(){
    std::cout << "\n=======================================\n";
    std::cout << "Help Page\n\n";
    std::cout << "    1. help         - this page.\n";
    std::cout << "    2. b2e2         - input like this will be parsed as a move.\n";
    std::cout << "    3. undo         - undo the previous move.\n";
    std::cout << "    4. exit or quit - exit the game.\n";
    std::cout << "    5. remake       - remake the game.\n";
    std::cout << "    6. advice       - give me a best move.\n\n";
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

    (void)getchar();
}

void start_game(){
    PieceSide userSide = PS_DOWN;
    PieceSide aiSide = PS_UP;

    ChessBoard cb;
    std::string userInput;

    print_board_to_console(cb);

    while (true){
        std::cout << "Your move: ";
        std::getline(std::cin, userInput);

        if (userInput == "help"){
            print_help_page();
            print_board_to_console(cb);
        }
        else if (userInput == "undo"){
            cb.undo();
            cb.undo();
            print_board_to_console(cb);
        }
        else if (userInput == "quit"){
            return;
        }
        else if (userInput == "exit"){
            return;
        }
        else if (userInput == "remake"){
            cb = ChessBoard{};

            std::cout << "New cnchess started.\n";
            print_board_to_console(cb);
            continue;
        }
        else if (userInput == "advice"){
            MoveNode advice = gen_best_move(cb, userSide, DEFAULT_AI_SEARCH_DEPTH);
            std::string adviceStr = convert_move_to_str(advice);
            std::cout << "Maybe you can try: " << adviceStr 
                        << ", piece is " << piece_get_char(cb.get(advice.beginRow, advice.beginCol))
                        << ".\n";
        }
        else{
            if (check_input_is_a_move(userInput)){
                MoveNode userMove = convert_input_to_move(userInput);
                
                if (!check_is_this_your_piece(cb, userMove, userSide)){
                    std::cout << "This piece is not yours, please choose your piece.\n";
                    continue;
                }

                if (check_rule(cb, userMove)){
                    cb.move(userMove);
                    print_board_to_console(cb);

                    if (check_winner(cb) == userSide){
                        std::cout << "Congratulations! You win!\n";
                        return;
                    }

                    std::cout << "AI thinking...\n";
                    MoveNode aiMove = gen_best_move(cb, aiSide, DEFAULT_AI_SEARCH_DEPTH);
                    std::string aiMoveStr = convert_move_to_str(aiMove);
                    cb.move(aiMove);
                    print_board_to_console(cb);
                    std::cout << "AI move: " << aiMoveStr
                             << ", piece is '" << piece_get_char(cb.get(aiMove.endRow, aiMove.endCol)) 
                             << "'.\n";

                    if (check_winner(cb) == aiSide){
                        std::cout << "Game over! You lose!\n";
                        return;
                    }
                }
                else {
                    std::cout << "Given move doesn't fit for rules, please re-enter.\n";
                    continue;
                }
            }
            else {
                std::cout << "Input is not a valid move nor instruction, please re-enter(try help ?).\n";
                continue;
            }
        }
    }
}

#endif
