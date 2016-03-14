#ifndef __BOARD_H__
#define __BOARD_H__

#include <bitset>
#include "common.h"
using namespace std;

class Board {
   
private:
    bitset<64> black;
    bitset<64> taken;
    //bitset<64> stable;
    
    int stableBlack;
       
    void set(Side side, int x, int y);
    bool onBoard(int x, int y);
    //void checkIfStable(int x, int y);
      
public:
    Board();
    ~Board();
    Board *copy();
        
    //bool isDone();
    //bool hasMoves(Side side);
    bool checkMove(int X, int Y, Side side);
    void doMove(Move *m, Side side);
    void doMove(int X, int Y, Side side);
    int count();
    int count(Side side);
    int countBlack();
    int countWhite();
    //int countStable(Side side);
    //int countStableBlack();
    //int countStableWhite();
    bool get(Side side, int x, int y);
    bool occupied(int x, int y);

    void setBoard(char data[]);
};

#endif
