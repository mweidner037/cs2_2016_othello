#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include <limits>
#include <cstdio>
#include <unordered_map>
#include "common.h"
#include "board.h"
using namespace std;

enum StartingMove { C5 = 0, C4 = 1, D3 = 2, E3 = 3, F4 = 4,
        F5 = 5, E6 = 6, D6 = 7 };

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
    
    long totalNodesSearched;
    int totalTranspositionHits;
    
    // movesSoFarString is normalized to start with C4.
    // The actual first move is described by startingMove.
   /* int movesSoFar;
    string movesSoFarString;
    StartingMove startingMove;*/
    
    int score(Board &currentBoard);
    bool applyMove(Board &currentBoard, int moveX, int moveY,
            Side side);
            
//    void setStartingMove(Move *move);
};

#endif
