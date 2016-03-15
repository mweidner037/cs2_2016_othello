#ifndef __COMMON_H__
#define __COMMON_H__

#define WHITE 1
#define BLACK 2
#define BLANK 0

using Side = int;

inline int flipSide(int side)
{
    return (side == BLACK)? WHITE: BLACK;
}

class Move {
   
public:
    int x, y;
    Move(int x, int y) {
        this->x = x;
        this->y = y;        
    }
    ~Move() {}

    int getX() { return x; }
    int getY() { return y; }

    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
};

#endif
