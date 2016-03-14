#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include <limits>
#include <cstdio>
#include "common.h"
#include "board.h"
using namespace std;

/*struct Board {
    TODO;
};*/

class Player {

public:
    Player(Side side);
    ~Player();
    
    Move *doMove(Move *opponentsMove, int msLeft);
    void setBoard(Board &board);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;

private:
    Side mySide;
    Board board;
    int score(Board &currentBoard);
    bool applyMove(Board &currentBoard, int moveX, int moveY,
            Side side);
    
    long totalNodesSearched;
};

#endif
