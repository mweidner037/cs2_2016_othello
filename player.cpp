#include "player.h"

#define MINIMAX_DEPTH 2

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish 
 * within 30 seconds.
 */
Player::Player(Side side) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;

    /* 
     * TODO: Do any initialization you need to do here (setting up the board,
     * precalculating things, etc.) However, remember that you will only have
     * 30 seconds.
     */
    mySide = side;
}

/*
 * Destructor for the player.
 */
Player::~Player() {
}

/**
 * @brief Set the internal board to a copy of the given board.
 */
void Player::setBoard(Board &board)
{
    this->board = board;
}

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be NULL.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return NULL.
 */
Move *Player::doMove(Move *opponentsMove, int msLeft) {
    /* 
     * TODO: Implement how moves your AI should play here. You should first
     * process the opponent's opponents move before calculating your own move
     */ 
    
    // record opponent's move
    board.doMove(opponentsMove, flipSide(mySide));
    
    // run mimimax
    Move *move = NULL;
    int score;
    minimax(board, mySide, false, MINIMAX_DEPTH, &score, &move);
    
    // make the chosen move (possibly NULL, if there are no legal moves)
    board.doMove(move, mySide);
    return move;
}

/**
 * @brief Apply minimax to currentBoard to the given depth, putting
 * the minimax score and move in scoreReturnValue and moveReturnValue.
 *
 * @param currentBoard The board to start with
 * @param turn The Side which makes the next move
 * @param minElseMax If true, find the move with min score, else
 *      find the move with max score
 * @param depth The depth to recurse to.  If <=0, just return the result
 * of getScore on the inputs.
 * @param scoreReturnValue Used to return the minimax score
 * @param moveReturnValue Used to return the minimax move
 **/
 
void Player::minimax(Board &currentBoard, Side turn, bool minElseMax,
        int depth, int *scoreReturnValue, Move **moveReturnValue)
{
    if (depth <= 0) {
        *(scoreReturnValue) = score(currentBoard, flipSide(turn));
        return;
    }
    
    Move *bestMoveSoFar = NULL;
    int bestScoreSoFar = (minElseMax? std::numeric_limits<int>::max():
            std::numeric_limits<int>::min());
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            Move *moveToTry = new Move(x, y);
            if (currentBoard.checkMove(moveToTry, turn))
            {
                // try out the move on a copy of currentBoard
                Board boardCopy = currentBoard;
                boardCopy.doMove(moveToTry, mySide);
                // recurse
                Move *bestMoveRecurse = NULL;
                int bestScoreRecurse;
                minimax(boardCopy, flipSide(turn), !minElseMax, depth - 1,
                        &bestScoreRecurse, &bestMoveRecurse);
                if ((minElseMax && bestScoreRecurse < bestScoreSoFar) ||
                        (!minElseMax && bestScoreRecurse > bestScoreSoFar))
                {
                    if (bestMoveSoFar) delete bestMoveSoFar;
                    bestScoreSoFar = bestScoreRecurse;
                    bestMoveSoFar = moveToTry;
                }
                else delete moveToTry;
            }
        }
    }
    
    //return the best move
    *(scoreReturnValue) = bestScoreSoFar;
    *(moveReturnValue) = bestMoveSoFar;
}

/**
 * @brief Score currentBoard for side (higher is better).
 */
int Player::score(Board &currentBoard, Side side)
{
    // return (# side's stones - #other side's stones)
    return (currentBoard.count(side) - currentBoard.count(flipSide(side)));
}
