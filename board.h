#ifndef __BOARD_H__
#define __BOARD_H__

#include <bitset>
#include <cstdint>
#include "common.h"
using namespace std;

class Board {
   
private:
    /* Each row is a 16-bit integer.  Lowest 2 bits represent x = 0
     * (see BLACK, WHITE, BLANK in common.h), highest 2 bits
     * represent x = 7.
     */
    uint16_t rows[8];
    int counts[3];
    
    int cachedBlackScore;
    bool cachedScoreValid;
       
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
    int score(Side side);
    //int countStable(Side side);
    //int countStableBlack();
    //int countStableWhite();
    Side get(int x, int y);

    void setBoard(char data[]);
};

#endif
