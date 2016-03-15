#include "board.h"

#define CORNER_BONUS 25

/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() {
    for (int i = 0; i < 8; i++) rows[i] = 0;
    counts[WHITE] = 0;
    counts[BLACK] = 0;
    
    set(WHITE, 3, 3);
    set(WHITE, 4, 4);
    set(BLACK, 4, 3);
    set(BLACK, 3, 4);
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
    for (int i = 0; i < 8; i++) newBoard->rows[i] = rows[i];
    newBoard->counts[WHITE] = counts[WHITE];
    newBoard->counts[BLACK] = counts[BLACK];
    return newBoard;
}

/* Returns the value at (x, y) (one of WHITE, BLACK, BLANK)
*/
Side Board::get(int x, int y) {
    return (rows[y] >> (2*x)) % 4;
}

void Board::set(Side side, int x, int y) {
    Side currentValue = get(x, y);
    if (currentValue == side) return;
    if (currentValue == flipSide(side)) counts[flipSide(side)]--;
    counts[side]++;
    cachedScoreValid = false;
    
    rows[y] &= ~(3 << (2*x));
    rows[y] |= side << (2*x);
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

inline bool Board::onBoard(int x, int y) {
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
    if (!onBoard(X, Y) || get(X, Y) != BLANK) return false;

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            // Is there a capture in that direction?
            int x = X + dx;
            int y = Y + dy;
            if (onBoard(x, y) && get(x, y) == flipSide(side)) {
                do {
                    x += dx;
                    y += dy;
                } while (onBoard(x, y) && get(x, y) == flipSide(side));

                if (onBoard(x, y) && get(x, y) == side) return true;
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

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            int x = X;
            int y = Y;
            do {
                x += dx;
                y += dy;
            } while (onBoard(x, y) && get(x, y) == flipSide(side));

            if (onBoard(x, y) && get(x, y) == side) {
                x = X;
                y = Y;
                x += dx;
                y += dy;
                while (onBoard(x, y) && get(x, y) == flipSide(side)) {
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
    return counts[WHITE] + counts[BLACK];
}

/*
 * Current count of given side's stones.
 */
int Board::count(Side side) {
    return counts[side];
}

/*
 * Current count of black stones.
 */
int Board::countBlack() {
    return counts[BLACK];
}

/*
 * Current count of white stones.
 */
int Board::countWhite() {
    return counts[WHITE];
}

/*
 * Score this board for side (higher is better).
 */
int Board::score(Side side) {
    if (cachedScoreValid)
    {
        return (side == BLACK)? cachedBlackScore: -cachedBlackScore;
    }
    
    int netBlackScore = countBlack() - countWhite();
    
    if (64 != count())
    {
        if (get(0, 0) == BLACK) netBlackScore += CORNER_BONUS;
        else if (get(0, 0) == WHITE) netBlackScore -= CORNER_BONUS;
        if (get(0, 7) == BLACK) netBlackScore += CORNER_BONUS;
        else if (get(0, 7) == WHITE) netBlackScore -= CORNER_BONUS;
        if (get(7, 0) == BLACK) netBlackScore += CORNER_BONUS;
        else if (get(7, 0) == WHITE) netBlackScore -= CORNER_BONUS;
        if (get(7, 7) == BLACK) netBlackScore += CORNER_BONUS;
        else if (get(7, 7) == WHITE) netBlackScore -= CORNER_BONUS;
    }
    
    cachedBlackScore = netBlackScore;
    cachedScoreValid = true;
    
    return (side == BLACK)? cachedBlackScore: -cachedBlackScore;
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
    for (int i = 0; i < 8; i++) rows[i] = 0;
    counts[WHITE] = 0;
    counts[BLACK] = 0;
    
    for (int i = 0; i < 64; i++) {
        if (data[i] == 'b') {
            set(BLACK, i % 8, i >> 3);
        } if (data[i] == 'w') {
            set(WHITE, i % 8, i >> 3);
        }
    }
}
