#include "player.h"

#define EARLY_MINIMAX_DEPTH 11
#define LATE_MINIMAX_DEPTH 25
#define TRANSPOSITION_DEPTH 6

const int WIN_SCORE = std::numeric_limits<int>::max() - 2;
const int LOSS_SCORE = std::numeric_limits<int>::min() + 2;

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
    //TODO: initialize board
    
    // initialize opening book
    movesSoFar = 0;
    movesSoFarString = "";
    // Load opening book from mweidner_data.dat.
    // Opening book copied from
    // http://www.samsoft.org.uk/reversi/openings.htm
    string line;
    ifstream myfile ("mweidner_data.dat");
    if (myfile.is_open())
    {
        while (getline (myfile,line) )
        {
            if (line.length() > 0)
                openingBook.push_back(line);
        }
        myfile.close();
    }
    else {
        cerr << "ERROR: Unable to load opening book";
    }
    
    totalNodesSearched = 0;
    totalTranspositionHits = 0;
}

/*
 * Destructor for the player.
 */
Player::~Player() {
}

/**
 * @brief Set the internal board to a copy of the given board.
 */
void Player::setBoard(Board &board)
{
    this->board = board;
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
    time_t startTime = time(nullptr);
    
    if (opponentsMove == NULL && movesSoFar == 0)
    {
        // first move; just go at C4
        Move *chosenMove = new Move(2, 3);
        applyMove(board, chosenMove->x, chosenMove->y, mySide);
                
        // opening book tracking
        setStartingMove(chosenMove);
        recordMove(chosenMove, mySide);
        
        return chosenMove;
    }
    
    // record opponent's move
    if (opponentsMove != NULL)
    {
        applyMove(board, opponentsMove->x, opponentsMove->y,
                flipSide(mySide));
                
        // opening book tracking
        if (movesSoFar == 0)
        {
            setStartingMove(opponentsMove);
        }
        recordMove(opponentsMove, flipSide(mySide));
    }
    
    // use the opening book if we can
    for (int i = 0; i < openingBook.size(); i++)
    {
        string opening = openingBook[i];
        if (opening.size() > movesSoFarString.size() &&
                opening.substr(0, movesSoFarString.size()) == movesSoFarString)
        {
            cerr << "Using opening: " << opening;
            string moveStr = opening.substr(movesSoFarString.size(), 2);
            cerr << " (move: " + moveStr + ": ";
            int x = (((int) moveStr[0]) - 1) % 32;
            int y = ((int) moveStr[1]) - 49;
            Move *normalizedMove = new Move(x, y);
            cerr << x << ", " << y << ")" << endl;
            Move *chosenMove = normalizedToReal(normalizedMove);
            applyMove(board, chosenMove->x, chosenMove->y, mySide);
            recordMove(chosenMove, mySide);
            movesSoFar++;
            
            delete normalizedMove;
            
            return chosenMove;
        }
    }
        
    int nodesSearched = 0;
    int transpositionHits = 0;
    
    // iterative deepening loop
    int chosenMoveIndex;
    int chosenMoveValue;
    
    bool finishedInTime = true;
    do {
        // run mimimax with alpha-beta pruning
        chosenMoveIndex = -1;
        
        unordered_map<Board, int> *transpositionTable =
                new unordered_map<Board, int>();
        
        // Don't do end-game depth search if we just tried that and
        // didn't finish in time.
        int maxDepth;
        if (64 - board.count() <= LATE_MINIMAX_DEPTH && finishedInTime)
        {
            maxDepth = LATE_MINIMAX_DEPTH;
        }
        else maxDepth = EARLY_MINIMAX_DEPTH;
        
        finishedInTime = true;
        
        int currentDepth = 0;
        int fromDepth = -1;
        
        Board *boards = new Board[2*(maxDepth + 2)];
        boards[0] = board;
        // Index moves from 0 to 63, with x in bits 1-3 and y in bits 4-6
        unsigned int *moveIndices = new unsigned int[2*(maxDepth + 1)];
        int *minimaxScores = new int[2*(maxDepth + 1)];
        int *alphas = new int[2*(maxDepth + 1)];
        int *betas = new int[2*(maxDepth + 1)];
        /* Scores of min/max indicate unset; scores of WIN_SCORE/LOSS_SCORE
         * indicate definite win/loss.
         */
        minimaxScores[0] = std::numeric_limits<int>::min();
        alphas[0] = std::numeric_limits<int>::min();
        betas[0] = std::numeric_limits<int>::max();
        
        Side currentSide = mySide;
        bool minElseMax = false;
        
        // We stop after two passes in a row, since then the game is over.
        int passesInARow = 0;
        
        // When maxDepth permits perfect play (without counting passes as moves),
        // play to the bottom of the search tree instead of just to maxDepth.
        // (If passes were impossible, perfectPlay would have no effect.)
        bool perfectPlay = (maxDepth == LATE_MINIMAX_DEPTH);
        if (perfectPlay) fprintf(stderr, "Perfect play...\n");
        
        // for timing purposes, count the number of legal moves
        int topLevelMoves = board.countMoves(mySide);
        int topLevelMovesSoFar = 0;
        
        while (true)
        {
            nodesSearched++;
            /*if (currentDepth <= 2 && fromDepth != -1)
            {
                fprintf(stderr, "\tEnd minimaxScore: %d\n", minimaxScores[fromDepth]);
                fprintf(stderr, "  Depth: %d, moveIndex: %d, minimaxScore: %d\n\talpha: %d, beta: %d\n",
                        currentDepth, moveIndices[currentDepth],
                        minimaxScores[currentDepth], alphas[currentDepth],
                        betas[currentDepth]);
            }*/
                    
            // tail "recursion"
            if ((!perfectPlay && currentDepth == maxDepth) ||
                    boards[currentDepth].count() == 64)
            {
                if (boards[currentDepth].count() == 64)
                {
                    // the board is full; record the winner
                    if (boards[currentDepth].count(mySide) > 32)
                    {
                        minimaxScores[currentDepth] = WIN_SCORE;
                    }
                    else if (boards[currentDepth].count(mySide) < 32)
                    {
                        minimaxScores[currentDepth] = LOSS_SCORE;
                    }
                    else minimaxScores[currentDepth] = 0;
                }
                else if (currentDepth >= 3)
                {
                    minimaxScores[currentDepth] =
                            boards[currentDepth].score(mySide) +
                            boards[currentDepth - 1].score(mySide) +
                            boards[currentDepth - 2].score(mySide) +
                            boards[currentDepth - 3].score(mySide);
                }
                else {
                    minimaxScores[currentDepth] = boards[currentDepth].score(mySide);
                }
                fromDepth = currentDepth;
                currentDepth--;
                currentSide = flipSide(currentSide);
                minElseMax = !minElseMax;
                continue;
            }
            
            // lookup in transposition table
            if (currentDepth == TRANSPOSITION_DEPTH)
            {
                unordered_map<Board, int>::const_iterator got =
                        transpositionTable->find(boards[currentDepth]);
                if (got != transpositionTable->end())
                {
                    // Found it in the transposition table; record and go
                    // back to parent.
                    transpositionHits++;
                    minimaxScores[currentDepth] = got->second;
                    fromDepth = currentDepth;
                    currentDepth--;
                    currentSide = flipSide(currentSide);
                    minElseMax = !minElseMax;
                    continue;
                }
            }
            
            if (fromDepth < currentDepth)
            {
                // we are starting visiting the current node, hence want to
                // traverse its first child edge (apply the first move)
                moveIndices[currentDepth] = 0;
                minimaxScores[currentDepth] =
                        (minElseMax)? std::numeric_limits<int>::max():
                        std::numeric_limits<int>::min();
                // down-propogate alpha and beta values
                if (currentDepth != 0)
                {
                    alphas[currentDepth] = alphas[currentDepth - 1];
                    betas[currentDepth] = betas[currentDepth - 1];
                }
            }
            else
            {
                if (fromDepth > currentDepth)
                {
                    // store the child that was just visited in transpositionTable
                    if (fromDepth == TRANSPOSITION_DEPTH)
                    {
                        (*transpositionTable)[boards[currentDepth + 1]] =
                                minimaxScores[currentDepth + 1];
                    }
                    
                    // If the child move that was just made minimaxes
                    // so far, record it.
                    if ((minElseMax && minimaxScores[currentDepth + 1] <
                            minimaxScores[currentDepth]) || (!minElseMax &&
                            minimaxScores[currentDepth + 1] >
                            minimaxScores[currentDepth]))
                    {
                        minimaxScores[currentDepth] =
                                minimaxScores[currentDepth + 1];
                        if (currentDepth == 0) {
                            // The child move just made is our overall chosen
                            // move (at least so far).
                            chosenMoveIndex = moveIndices[0];
                            chosenMoveValue = minimaxScores[0];
                        }
                        
                        // If this node is now a guaranteed win for one player,
                        // prune the rest of its children, by going to the parent.
                        if ((minElseMax &&
                                minimaxScores[currentDepth] == LOSS_SCORE) ||
                                (!minElseMax &&
                                minimaxScores[currentDepth] == WIN_SCORE))
                        {
                            if (currentDepth == 0) break;
                            else {
                                fromDepth = currentDepth;
                                currentDepth--;
                                currentSide = flipSide(currentSide);
                                minElseMax = !minElseMax;
                                continue;
                            }
                        }
                        
                        // up-propogate alpha and beta values
                        if (minElseMax)
                        {
                            betas[currentDepth] = min(betas[currentDepth],
                                    minimaxScores[currentDepth]);
                        }
                        else {
                            alphas[currentDepth] = max(alphas[currentDepth],
                                    minimaxScores[currentDepth]);
                        }
                        
                        // alpha-beta prune
                        if (alphas[currentDepth] > betas[currentDepth])
                        {
                            /*fprintf(stderr, "Pruned: depth=%d, alpha=%d, beta=%d.\n",
                                    currentDepth, alphas[currentDepth],
                                    betas[currentDepth]);*/
                            // This node is pruned; go to the parent.
                            if (currentDepth == 0) break;
                            else {
                                fromDepth = currentDepth;
                                currentDepth--;
                                currentSide = flipSide(currentSide);
                                minElseMax = !minElseMax;
                                continue;
                            }
                        }
                    }
                }
                // else the last child move made was invalid; don't record it
                
                // we are moving to the next child edge (move) of this node
                moveIndices[currentDepth]++;
                if (moveIndices[currentDepth] >= 64)
                {
                    // We have tried all possible moves on child edges.
                    // Thus we are done traversing this node unless there
                    // were no valid moves, in which case we need to explore
                    // a new child edge, representing the move "pass".
                    if (currentDepth != 0 && (
                            (minElseMax && minimaxScores[currentDepth] ==
                            std::numeric_limits<int>::max()) ||
                            (!minElseMax && minimaxScores[currentDepth] ==
                            std::numeric_limits<int>::min())))
                    {
                        // Need to explore a new child edge, for the move "pass",
                        // unless doing so would involve both players passing,
                        // in which case the game is over.
                        passesInARow++;
                        if (passesInARow > 2)
                            fprintf(stderr, "ERROR: too many passes\n");
                        if (passesInARow >= 2)
                        {
                            // the game ended at this node; record the result
                            if (board.count(mySide) >
                                    board.count(flipSide(mySide)))
                            {
                                minimaxScores[currentDepth] = WIN_SCORE;
                            }
                            else if (board.count(mySide) <
                                    board.count(flipSide(mySide)))
                            {
                                minimaxScores[currentDepth] = LOSS_SCORE;
                            }
                            else minimaxScores[currentDepth] = 0;
                            // now go back to the parent
                            passesInARow = 0;
                            fromDepth = currentDepth;
                            currentDepth--;
                            currentSide = flipSide(currentSide);
                            minElseMax = !minElseMax;
                            continue;
                        }
                        
                        // Board doesn't change.
                        boards[currentDepth + 1] = boards[currentDepth];
                        // "Recurse"
                        fromDepth = currentDepth;
                        currentDepth++;
                        currentSide = flipSide(currentSide);
                        minElseMax = !minElseMax;
                        continue;
                    }
                    else {
                        // We are done traversing this node's child edges;
                        // go to the parent.  Note that even if no child edges
                        // had valid moves, we will enter this case after
                        // exploring the "pass" child edge, since exploring
                        // that edge will set minimaxScores[currentDepth + 1]
                        // to a non-extreme value.
                        if (currentDepth == 0) break;
                        else {
                            fromDepth = currentDepth;
                            currentDepth--;
                            currentSide = flipSide(currentSide);
                            minElseMax = !minElseMax;
                            continue;
                        }
                    }
                }
            }
            
            // Make the move corresponding to moveIndex[currentDepth].
            boards[currentDepth + 1] = boards[currentDepth];
            if (applyMove(boards[currentDepth + 1],
                    moveIndices[currentDepth] % 8, moveIndices[currentDepth] >> 3,
                    currentSide))
            {
                // Move was valid; "Recurse"
                if (currentDepth == 0) // record timing info
                {
                    topLevelMovesSoFar++;
                    // check if we are going to finish in time
                    if (perfectPlay && !haveTimeForPerfection(topLevelMovesSoFar,
                            topLevelMoves, msLeft, startTime))
                    {
                        // not enough time; go through the iterative
                        // deepening loop a second time, this time
                        // with the early game depth.
                        finishedInTime = false;
                        cerr << "NOTICE: Not going to finish in time; drop " <<
                                "to non-perfection." << endl;
                        break;
                    }
                }
                
                passesInARow = 0;
                fromDepth = currentDepth;
                currentDepth++;
                currentSide = flipSide(currentSide);
                minElseMax = !minElseMax;
            }
            else {
                // Can't actually make this move; move to the next child of
                // this node.
                fromDepth = currentDepth;
            }
        }
    
        delete[] boards;
        delete[] moveIndices;
        delete[] minimaxScores;
        delete[] alphas;
        delete[] betas;
        delete transpositionTable;
    } while(!finishedInTime);
    
    // make the chosen move (possibly NULL, if there are no legal moves)
    Move *chosenMove = NULL;
    if (chosenMoveIndex != -1)
    {
        chosenMove = new Move(chosenMoveIndex % 8, chosenMoveIndex >> 3);
        applyMove(board, chosenMove->x, chosenMove->y, mySide);
        fprintf(stderr, "Move value: %d\n", chosenMoveValue);
        
        if (movesSoFar == 0)
        {
            setStartingMove(chosenMove);
        }
        recordMove(chosenMove, mySide);
        movesSoFar++;
    }
    
    totalTranspositionHits += transpositionHits;
    fprintf(stderr, "Transposition hits (local/total): %d/%d\n",
            transpositionHits, totalTranspositionHits);
            
    totalNodesSearched += nodesSearched;
    fprintf(stderr, "Nodes searched (local/total): %d/%li\n",
            nodesSearched, totalNodesSearched);
            
    fprintf(stderr, "msLeft: %d\n", msLeft);
    
    return chosenMove;
}

// returns true iff the given move is valid
bool Player::applyMove(Board &currentBoard, int moveX, int moveY,
        Side side)
{
    if (currentBoard.checkMove(moveX, moveY, side))
    {
        currentBoard.doMove(moveX, moveY, side);
        return true;
    }
    else {
        return false;
    }
}

bool Player::haveTimeForPerfection(int topLevelMovesSoFar, int topLevelMoves,
        int msLeft, time_t startTime)
{
    cerr << "checking" << endl;
    
    int msUsed = 1000 * (time(nullptr) - startTime);
    // we expect the first perfection to use <= 2/3 of the remaining time
    int allottedMs = (2 * msLeft) / 3;
    int targetMs = (int) (allottedMs * ((double)topLevelMovesSoFar) /
            ((double) topLevelMoves));
    /*cerr << "Allotted: " << allottedMs << "; Target: " << targetMs <<
            "; used: " << msUsed;*/
    return (msUsed < targetMs);
}


void Player::setStartingMove(Move *move)
{
    if (move->x == 2 && move->y == 4) startingMove = C5;
    if (move->x == 2 && move->y == 3) startingMove = C4;
    if (move->x == 3 && move->y == 2) startingMove = D3;
    if (move->x == 4 && move->y == 2) startingMove = E3;
    if (move->x == 5 && move->y == 3) startingMove = F4;
    if (move->x == 5 && move->y == 4) startingMove = F5;
    if (move->x == 4 && move->y == 5) startingMove = E6;
    if (move->x == 3 && move->y == 5) startingMove = D6;
}

void Player::recordMove(Move *move, Side side)
{
    Move *normalized = realToNormalized(move);
    char letter = (side == BLACK)? 64 + (normalized->x + 1):
            96 + (normalized->x + 1);
    char number = 49 + normalized->y;
    movesSoFarString.push_back(letter);
    movesSoFarString.push_back(number);
    movesSoFar++;
    
    //cerr << movesSoFarString << endl;
    
    //usleep(10000000);
}

Move *Player::realToNormalized(Move *real)
{
    switch (startingMove)
    {
        case C5: return new Move(real->x, 7 - real->y);
        case C4: return new Move(real->x, real->y);
        case D3: return new Move(real->y, real->x);
        case E3: return new Move(real->y, 7 - real->x);
        case F4: return new Move(7 - real->x, real->y);
        case F5: return new Move(7 - real->x, 7 - real->y);
        case E6: return new Move(7 - real->y, 7 - real->x);
        case D6: return new Move(7 - real->y, real->x);
        default:
            fprintf(stderr, "ERROR: no normalization");
            return NULL;
    }
}

Move *Player::normalizedToReal(Move *normalized)
{
    // lazy hack
    Move *tryMove = new Move(0, 0);
    for (int flip = 0; flip <= 1; flip++)
    {
        for (int negX = 0; negX <= 1; negX++)
        {
            for (int negY = 0; negY <= 1; negY++)
            {
                if (flip)
                {
                    tryMove->x = normalized->y;
                    tryMove->y = normalized->x;
                }
                else {
                    tryMove->x = normalized->x;
                    tryMove->y = normalized->y;
                }
                if (negX) tryMove->x = 7 - tryMove->x;
                if (negY) tryMove->y = 7 - tryMove->y;
                Move *realTryMove = realToNormalized(tryMove);
                if (realTryMove->x == normalized->x &&
                        realTryMove->y == normalized->y)
                {
                    return tryMove;
                }
                else delete realTryMove;
            }
        }
    }
    
    fprintf(stderr, "ERROR: no inverse");
    return NULL;
}
