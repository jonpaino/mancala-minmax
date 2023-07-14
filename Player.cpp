#include <iostream>
#include "Player.h"
using namespace std;

//ABSTRACT BASE CLASS IMPLEMENTATIONS
Player::Player(string name) : m_name(name) {}
std::string Player::name() const { return m_name; }
bool Player::isInteractive() const { return false; }
Player::~Player() { }

//DERIVED HUMANPLAYER CLASS IMPLEMENTATIONS
HumanPlayer::HumanPlayer(string name) : Player(name) {}
bool HumanPlayer::isInteractive() const { return true; }
int HumanPlayer::chooseMove(const Board& b, Side s) const
{
    if(b.beansInPlay(s) == 0) return -1; //Can't make a move
    int move;
    while(true){
        cout  << Player::name() << " please pick a move: ";
        cin >> move;
        if(move > 0 && move <= b.holes() && b.beans(s,move) > 0) break; //Leave infinite loop once valid input was made
        if(move <= 0 || move > b.holes())
        {
            cout << "Invalid move. Select a hole number from 1 to " << b.holes() << endl;
        }
        else
            cout << "Not enough beans to make move!" << endl;
    }
    return move;
}
HumanPlayer::~HumanPlayer() {}


//DERIVED BADPLAYER IMPLEMENTATION
BadPlayer::BadPlayer(string name) : Player(name) {}
int BadPlayer::chooseMove(const Board& b, Side s) const
{
    if(b.beansInPlay(s) == 0) return -1;
    int move = -1;
    for(int i = 1; i <= b.holes(); i++){ //Returns the first playable hole starting from 1
        if(b.beans(s,i) > 0){ 
        move = i;
        break;
    }
    }
    cout << Player::name() << " chooses hole " << move << endl;
    return move;
}
BadPlayer::~BadPlayer() { }


//DERIVED SMARTPLAYER IMPLEMENTATION
SmartPlayer::SmartPlayer(string name) : Player(name) {}

int SmartPlayer::chooseMove(const Board& b, Side s) const
{
    double timeLimit = 5000;  // 4.99 seconds; allow 10 ms for wrapping up
    Timer timer;
    //Set the best possible move to some negative number
    //Create a new int variable to track the curent move value
    //Loop through each playable move and call the minimax function for each move
    int bestMove = -100000;
    int newMove = 0;
    int move = 0; // We need a hole number to return not a value
    for(int i = 1; i <= b.holes(); i++){
        if(b.beans(s,i) > 0){
            Board tBoard(b);
            bool newIsMax;
            bool canGoAgain = goAgain(tBoard,i,s);
            if(canGoAgain){
                newIsMax = true;
                newMove = minimax(tBoard,s,newIsMax,m_depth,timeLimit,timer);
            } else{
                newIsMax = false;
                newMove = minimax(tBoard,s,newIsMax,m_depth,timeLimit,timer);
            }
            if(newMove >= bestMove){
                bestMove = newMove;
                move = i;
            }
        }
    }
    cout << timer.elapsed() << " ms" << endl;
    cout << Player::name() << " chose move " << move << endl;
    return move;
}

int SmartPlayer::eval(const Board& board, Side side) const
{
    if(board.beansInPlay(side) == 0 || board.beansInPlay(opponent(side)) == 0){ // if game is over
        //Having more total beans than the other player should be worth some eval points
        if(board.beansInPlay(side) + board.beans(side, 0) > board.beansInPlay(opponent(side)) + board.beans(opponent(side), 0)){
            return 1000; //Good situation for maximizing player
        }
        else if(board.beansInPlay(side) + board.beans(side, 0) < board.beansInPlay(opponent(side)) + board.beans(opponent(side), 0)){
            return -1000; //Bad situation for maximizing player
        }
        else return 0; //Currently a tie
    }
    //Having a majority of the beans in a pot should also be worth some points
    if(board.beans(side,0) > board.totalBeans()/2){
        return 1000;
    }
    else if(board.beans(opponent(side),0) > board.totalBeans()/2){
        return -1000;
    }
    //The difference in beans in the pot should be worth some points
    return board.beans(side,0) - board.beans(opponent(side), 0);
}
// The minimax function needs a way of determining if the player should go again
bool SmartPlayer::goAgain(Board& board, int move, Side side) const
{
    Side endSide;
    int endHole;
    bool canSow = board.sow(side,move,endSide,endHole);
    if(!canSow) { return false; }//Can't move at currenet board state
    if(side == endSide && board.beans(endSide,endHole) == 1 && board.beans(opponent(endSide),endHole) > 0){ 
        board.moveToPot(side, endHole, side); // If sow results in capture
        board.moveToPot(opponent(side),endHole,side);
        return false; // Can't move again
    }
    if(endSide == side && endHole == 0){
        return true; // Can move again
    }
    return false; // Can't move again
}
 int SmartPlayer::minimax(const Board& board, Side side, bool isMax ,int depth, double timeLimit, Timer& timer) const
{
    // If the current state of the board is end game return the current evaluation of its state
    if(board.beansInPlay(side) == 0 || board.beansInPlay(opponent(side)) == 0 || depth == 0)
    {
        return eval(board, side);
    }
    int bestHoleVal;
    if(isMax){ // If its the maximizing players turn
        bestHoleVal = -100000; //Set best move to a very low value
        //Go through every possible move for maximizing player
        //If maximizing player can go again recursively call minimax with the newisMax position set to true
        //If maximizing player can not go again call minimax with the newisMax position set to false
        for(int i = 1; i <= board.holes(); i++){
            if(board.beans(side, i) > 0){
                Board tBoard(board); //Performing each move on a temp board
                bool newIsMax;
                int newBestHole;
                bool canGoAgain = goAgain(tBoard, i, side);
                if(canGoAgain){
                    newIsMax = true;
                     // Allocate remaining time limit equally among remaining braches to explore
            double thisBranchTimeLimit = timeLimit / board.holes();
              // Explore this branch
                    newBestHole = minimax(tBoard,side,newIsMax,depth -1,thisBranchTimeLimit,timer);
                } else{
                    newIsMax = false;
                     // Allocate remaining time limit equally among remaining braches to explore
            double thisBranchTimeLimit = timeLimit / board.holes();
              // Explore this branch
                    newBestHole = minimax(tBoard,opponent(side),newIsMax,depth -1,thisBranchTimeLimit,timer);
                }
                if(newBestHole >= bestHoleVal){
                    bestHoleVal = newBestHole;
                }
            }
        }
    }else{ //Is minimizing players turn
       bestHoleVal = 100000; //Set the best hole value to some very large value
       for(int i = 1; i <= board.holes(); i++){
            if(board.beans(side,i) > 0){
                Board tBoard(board);
                bool canGoAgain = goAgain(tBoard, i, side);
                bool newIsMax;
                int newMinHole;
                if(canGoAgain){
                    newIsMax = false; // Minimizing player can go again
                        // Allocate remaining time limit equally among remaining braches to explore
            double thisBranchTimeLimit = timeLimit / board.holes();
              // Explore this branch
                    newMinHole = minimax(tBoard,opponent(side),newIsMax,depth -1,thisBranchTimeLimit,timer); // Want to find best move from maximizing players perspective before going again
                } else{ // Minimizing player can't make another move
                    newIsMax = true; // Now maximizing players turn
                        // Allocate remaining time limit equally among remaining braches to explore
            double thisBranchTimeLimit = timeLimit / board.holes();
              // Explore this branch
                    newMinHole = minimax(tBoard,opponent(side),newIsMax,depth -1,thisBranchTimeLimit,timer);
                }
                if(bestHoleVal >= newMinHole){
                    bestHoleVal = newMinHole;
                }
            }    
       }
    }
    return bestHoleVal;
}


SmartPlayer::~SmartPlayer() { }
