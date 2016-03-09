#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include <limits>
#include "common.h"
#include "board.h"
using namespace std;

class Player {

public:
    Player(Side side);
    ~Player();
    
    void setBoard(Board &board);
    
    Move *doMove(Move *opponentsMove, int msLeft);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;

private:
    Side mySide;
    Board board;
    void minimax(Board &currentBoard, Side turn, bool minElseMax,
            int depth, int *scoreReturnValue, Move **moveReturnValue);
    int score(Board &currentBoard, Side side);
};

#endif
