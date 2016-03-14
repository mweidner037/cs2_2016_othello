#include "board.h"

/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() {
    taken.set(3 + 8 * 3);
    taken.set(3 + 8 * 4);
    taken.set(4 + 8 * 3);
    taken.set(4 + 8 * 4);
    black.set(4 + 8 * 3);
    black.set(3 + 8 * 4);
    
    stableBlack = 0;
}

/*
 * Destructor for the board.
 */
Board::~Board() {
}

/*
 * Returns a copy of this board.
 */
Board *Board::copy() {
    Board *newBoard = new Board();
    newBoard->black = black;
    newBoard->taken = taken;
    return newBoard;
}

bool Board::occupied(int x, int y) {
    return taken[x + 8*y];
}

bool Board::get(Side side, int x, int y) {
    return occupied(x, y) && (black[x + 8*y] == (side == BLACK));
}

void Board::set(Side side, int x, int y) {
    taken.set(x + 8*y);
    black.set(x + 8*y, side == BLACK);
    // check if it's stable
//    if (!stable[x + 8*y]) checkIfStable(x, y);
}

/*// Checks whether the taken space (x, y) is stable, and if so,
// also updates neighbor's stability.
void Board::checkIfStable(int x, int y)
{
    Side side = (black[x + 8*y])? BLACK: WHITE;
    // horizontal check
    if (x == 0 || x == 7 ||
            (get(side, x - 1, y) && stable[(x-1) + 8*y]) ||
            (get(side, x + 1, y) && stable[(x+1) + 8*y]))
    {
        // vertical check
        if (y == 0 || y == 7 ||
                (get(side, x, y - 1) && stable[x + 8*(y-1)]) ||
                (get(side, x, y + 1) && stable[x + 8*(y+1)]))
        {
            // diagonal check
            if (y == 0 || y == 7 || x == 0 || x == 7 ||
                    (get(side, x - 1, y - 1) && stable[(x-1) + 8*(y-1)]) ||
                    (get(side, x - 1, y + 1) && stable[(x-1) + 8*(y+1)]) ||
                    (get(side, x + 1, y - 1) && stable[(x+1) + 8*(y-1)]) ||
                    (get(side, x + 1, y + 1) && stable[(x+1) + 8*(y+1)]))
            {
                // this position is stable
                if (side == BLACK) stableBlack++;
                stable.set(x + 8*y);
                // update neighbors
                *//*for (int newX = x - 1; newX <= x + 1; x += 2)
                {
                    for (int newY = y - 1; newY <= y + 1; y += 2)
                    {
                        if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8
                                && get(side, newX, newY)
                                && !stable[newX + 8*newY])
                        {
                            checkIfStable(newX, newY);
                        }
                    }
                }*//*
            }
        }
    }
}*/

bool Board::onBoard(int x, int y) {
    return(0 <= x && x < 8 && 0 <= y && y < 8);
}

 
/*
 * Returns true if the game is finished; false otherwise. The game is finished 
 * if neither side has a legal move.
 */
/*bool Board::isDone() {
    return !(hasMoves(BLACK) || hasMoves(WHITE));
}*/

/*
 * Returns true if there are legal moves for the given side.
 */
/*bool Board::hasMoves(Side side) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Move move(i, j);
            if (checkMove(&move, side)) return true;
        }
    }
    return false;
}*/

/*
 * Returns true if a move is legal for the given side; false otherwise.
 */
bool Board::checkMove(int X, int Y, Side side) {
    // Make sure the square hasn't already been taken.
    if (occupied(X, Y)) return false;

    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            // Is there a capture in that direction?
            int x = X + dx;
            int y = Y + dy;
            if (onBoard(x, y) && get(other, x, y)) {
                do {
                    x += dx;
                    y += dy;
                } while (onBoard(x, y) && get(other, x, y));

                if (onBoard(x, y) && get(side, x, y)) return true;
            }
        }
    }
    return false;
}

/*
 * Modifies the board to reflect the specified move.  Assumes that
 * the move is pre-checked if it is non-NULL.
 */
void Board::doMove(Move *m, Side side) {
    // A NULL move means pass.
    if (m == NULL) return;
    doMove(m->x, m->y, side);
}

/*
 * Modifies the board to reflect the specified move.  Assumes that
 * the move is pre-checked if it is non-NULL.
 */
void Board::doMove(int X, int Y, Side side)
{
    /*// Ignore if move is invalid.
    if (!checkMove(m, side)) return;*/

    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            int x = X;
            int y = Y;
            do {
                x += dx;
                y += dy;
            } while (onBoard(x, y) && get(other, x, y));

            if (onBoard(x, y) && get(side, x, y)) {
                x = X;
                y = Y;
                x += dx;
                y += dy;
                while (onBoard(x, y) && get(other, x, y)) {
                    set(side, x, y);
                    x += dx;
                    y += dy;
                }
            }
        }
    }
    set(side, X, Y);
}

/*
 * Current count of total stones.
 */
int Board::count() {
    return taken.count();
}

/*
 * Current count of given side's stones.
 */
int Board::count(Side side) {
    return (side == BLACK) ? countBlack() : countWhite();
}

/*
 * Current count of black stones.
 */
int Board::countBlack() {
    return black.count();
}

/*
 * Current count of white stones.
 */
int Board::countWhite() {
    return taken.count() - black.count();
}

/*
 * Current count of given side's stable stones.  Only accurate if stones
 * have never been removed from the board.
 */
/*int Board::countStable(Side side) {
    return (side == BLACK) ? countStableBlack() : countStableWhite();
}*/

/*
 * Current count of stable black stones.  Only accurate if stones
 * have never been removed from the board.
 */
/*int Board::countStableBlack()
{
    return stableBlack;
}*/

/*
 * Current count of stable white stones.  Only accurate if stones
 * have never been removed from the board.
 */
/*int Board::countStableWhite()
{
    return stable.count() - stableBlack;
}*/

/*
 * Sets the board state given an 8x8 char array where 'w' indicates a white
 * piece and 'b' indicates a black piece. Mainly for testing purposes.
 */
void Board::setBoard(char data[]) {
    taken.reset();
    black.reset();
    for (int i = 0; i < 64; i++) {
        if (data[i] == 'b') {
            taken.set(i);
            black.set(i);
        } if (data[i] == 'w') {
            taken.set(i);
        }
    }
}
