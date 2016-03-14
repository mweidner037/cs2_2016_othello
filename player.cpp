#include "player.h"

#define MINIMAX_DEPTH 9
#define CORNER_BONUS 100

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
    
    totalNodesSearched = 0;
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
    
    // record opponent's move
    if (opponentsMove != NULL)
    {
        applyMove(board, opponentsMove->x, opponentsMove->y,
                flipSide(mySide));
    }
    
    //std::cerr << "Starting minimax" << std::endl;
    
    // run mimimax with alpha-beta pruning
    int maxDepth = MINIMAX_DEPTH;
    int currentDepth = 0;
    int fromDepth = -1;
    Board *boards = new Board[maxDepth + 1];
    boards[0] = board;
    // Index moves from 0 to 63, with x in bits 1-3 and y in bits 4-6
    unsigned int *moveIndices = new unsigned int[maxDepth + 1];
    int *minimaxScores = new int[maxDepth + 1];
    int *alphas = new int[maxDepth + 1];
    int *betas = new int[maxDepth + 1];
    minimaxScores[0] = std::numeric_limits<int>::min();
    alphas[0] = std::numeric_limits<int>::min();
    betas[0] = std::numeric_limits<int>::max();
    Side currentSide = mySide;
    bool minElseMax = false;
    int chosenMoveIndex = -1;
    
    int nodesSearched = 0;
    
    while (true)
    {
        nodesSearched++;
        /*fprintf(stderr, "  Depth: %d, moveIndex: %d, minimaxScore: %d\n\talpha: %d, beta: %d\n",
                currentDepth, moveIndices[currentDepth],
                minimaxScores[currentDepth], alphas[currentDepth],
                betas[currentDepth]);*/
                
        // tail "recursion"
        if (currentDepth == maxDepth)
        {
            minimaxScores[currentDepth] = score(boards[currentDepth]);
            fromDepth = currentDepth;
            currentDepth--;
            currentSide = flipSide(currentSide);
            minElseMax = !minElseMax;
            continue;
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
                    }
                    
                    // If this node is now a guaranteed win for one player,
                    // prune it.
                    // Note we must allow twice as many moves as open
                    // spots due to passing.
                    /*if (((minElseMax && minimaxScores[currentDepth] < 0) ||
                            (!minElseMax && minimaxScores[currentDepth] > 0))
                            && (1 * (64 - boards[currentDepth].count()) <=
                            maxDepth - currentDepth))
                    {
                        // This node is pruned; go to the parent after
                        // marking this node as certain victory.
                        minimaxScores[currentDepth] = (minElseMax)?
                                std::numeric_limits<int>::min() + 1:
                                std::numeric_limits<int>::max() - 1;
                        if (currentDepth == 0) break;
                        else {
                            fromDepth = currentDepth;
                            currentDepth--;
                            currentSide = flipSide(currentSide);
                            minElseMax = !minElseMax;
                            continue;
                        }
                    }*/
                    
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
                    // Need to explore a new child edge, for the move "pass".
                    
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
    
    // make the chosen move (possibly NULL, if there are no legal moves)
    Move *chosenMove = NULL;
    if (chosenMoveIndex != -1)
    {
        chosenMove = new Move(chosenMoveIndex % 8, chosenMoveIndex >> 3);
        applyMove(board, chosenMove->x, chosenMove->y, mySide);
    }
    
    totalNodesSearched += nodesSearched;
    fprintf(stderr, "Nodes searched: %d\n", nodesSearched);
    fprintf(stderr, "Total nodes searched: %li\n", totalNodesSearched);
    
    delete moveIndices;
    delete minimaxScores;
    delete alphas;
    delete betas;
    
    return chosenMove;
}

/**
 * @brief Score currentBoard for mySide (higher is better).
 */
inline int Player::score(Board &currentBoard)
{
    int netBlackCount = currentBoard.countBlack() - currentBoard.countWhite();
    
    if (64 != currentBoard.count())
    {
        if (currentBoard.get(BLACK, 0, 0)) netBlackCount += CORNER_BONUS;
        else if (currentBoard.get(WHITE, 0, 0)) netBlackCount -= CORNER_BONUS;
        if (currentBoard.get(BLACK, 0, 7)) netBlackCount += CORNER_BONUS;
        else if (currentBoard.get(WHITE, 0, 7)) netBlackCount -= CORNER_BONUS;
        if (currentBoard.get(BLACK, 7, 0)) netBlackCount += CORNER_BONUS;
        else if (currentBoard.get(WHITE, 7, 0)) netBlackCount -= CORNER_BONUS;
        if (currentBoard.get(BLACK, 7, 7)) netBlackCount += CORNER_BONUS;
        else if (currentBoard.get(WHITE, 7, 7)) netBlackCount -= CORNER_BONUS;
    }
    
    return (mySide == BLACK)? netBlackCount: -netBlackCount;
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
