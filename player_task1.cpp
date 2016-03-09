/* player.cpp for task 1 of assignment part 1 (build a working
* player).  To compile, rename this file to player.cpp
* and make.
*/

#include "player.h"

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
    
    // make the first legal move I can find
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            Move *moveToTry = new Move(x, y);
            if (board.checkMove(moveToTry, mySide))
            {
                board.doMove(moveToTry, mySide);
                return moveToTry;
            }
        }
    }
    
    // no legal moves are available; pass
    board.doMove(NULL, mySide);
    return NULL;
}
