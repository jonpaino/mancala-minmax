#include <string>
#include "Board.h"
#include "chrono"

#ifndef PLAYER_H
#define PLAYER_H


class Timer
{
  public:
    Timer()
    {
        start();
    }
    void start()
    {
        m_time = std::chrono::high_resolution_clock::now();
    }
    double elapsed() const
    {   
        std::chrono::duration<double, std::milli> diff =
                          std::chrono::high_resolution_clock::now() - m_time;
        return diff.count();
    }
  private:
    std::chrono::high_resolution_clock::time_point m_time;
};

class Player
{
public:
    Player(std::string name);
    //Create a Player with the indicated name.
    std::string name() const;
    //Return the name of the player.
    virtual bool isInteractive() const;
    //Return false if the player is a computer player. Return true if the player is human. 
    //Most kinds of players will be computer players.
    virtual int chooseMove(const Board& b, Side s) const = 0;
    //Every concrete class derived from this class must implement this function so that 
    //if the player were to be playing side s and had to make a move given board b, 
    //the function returns the move the player would choose. If no move is possible, return −1.
    virtual ~Player();
    //Since this class is designed as a base class, it should have a virtual destructor.
private:
    std::string m_name;
};

//A HumanPlayer choosmes its move by prompting a person running the program for a move 
//(reprompting if necessary until the person enters a valid hole number), and returning that choice. 
//We will never test for a situation where the user doesn't enter an integer when prompted for a hole number. 
//(The techniques for dealing with the issue completely correctly are a distraction to this project, 
//and involve either a function like stoi or strtol, or the type istringstream.)
class HumanPlayer : public Player
{
public:
    HumanPlayer(std::string name);
    virtual bool isInteractive() const;
    virtual int chooseMove(const Board& b, Side s) const;
    virtual ~HumanPlayer();
};

class BadPlayer : public Player
{
public:
    BadPlayer(std::string name);
    virtual int chooseMove(const Board& b, Side s) const;
    virtual ~BadPlayer();
};

class SmartPlayer : public Player
{
public:
    SmartPlayer(std::string name);
    virtual int chooseMove(const Board& b, Side s) const;
    virtual ~SmartPlayer();
private:
    int eval(const Board& board, Side side) const; // arbitrarily determines value of a given position for a certain player
    bool goAgain(Board& board, int move, Side side) const; //return true if make another move
    int minimax(const Board& board, Side side, bool isMax ,int depth, double timeLimit, Timer& timer) const; // return value for a certain move
    int m_depth = 8;
};

#endif