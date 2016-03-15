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
        
    uint16_t rows[8];
    
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
    
    bool operator==(const Board &other) const;

    void setBoard(char data[]);
};

// based off of http://stackoverflow.com/questions/17016175/
// c-unordered-map-using-a-custom-class-type-as-the-key
namespace std {

    template <>
    struct hash<Board>
    {
        std::size_t operator()(const Board& b) const
        {
            return b.rows[3] + (b.rows[2] << 8) + (b.rows[1] << 16) +
                    (b.rows[0] << 24);
        }
    };
}

#endif
