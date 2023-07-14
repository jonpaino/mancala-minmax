#include <iostream>
#include "Game.h"
using namespace std;

Game::Game(const Board& b, Player* south, Player* north) 
    : m_south(south), m_north(north), m_board(b) {}

void Game::display() const
{
    cout << "        " << m_north->name()  << endl << "      ";
    for(int i = 1; i <= m_board.holes(); i++){
        cout << m_board.beans(NORTH,i) << " ";
    }
    cout << endl << "  " << m_board.beans(NORTH, 0); 
    for(int i = 0; i <= m_board.holes(); i++) // outputs the spaces
    {
        std::cout<< "  ";
    }
    cout << "  " << m_board.beans(SOUTH, 0) << endl << "      ";;
    for(int i = 1; i <= m_board.holes(); i++){
        cout << m_board.beans(SOUTH,i) << " ";
    }
    cout << "     " << endl << "        " << m_south->name()  << endl;
}
    //Display the game's board in a manner of your choosing, provided you show the names of the players 
    //and a reasonable representation of the state of the board.

void Game::status(bool& over, bool& hasWinner, Side& winner) const
{
    // Still a playable board
    if(m_board.beansInPlay(SOUTH) > 0 && m_board.beansInPlay(NORTH) > 0){ 
        over = false;
        return;
    }
    // When its not playable and then beans in play + pot for one side is greater set winner to that side and over to true
    if(m_board.beans(NORTH, 0) + m_board.beansInPlay(NORTH) > m_board.beans(SOUTH, 0) + m_board.beansInPlay(SOUTH)){
        over = true;
        hasWinner = true;
        winner = NORTH;
        return;
    }
    // South wins
    if(m_board.beans(NORTH, 0) + m_board.beansInPlay(NORTH) < m_board.beans(SOUTH, 0) + m_board.beansInPlay(SOUTH)){
        over = true;
        hasWinner = true;
        winner = SOUTH;
        return;
    }
    // When the board is equal in beans on each side
    if(m_board.beans(NORTH, 0) + m_board.beansInPlay(NORTH) == m_board.beans(SOUTH, 0) + m_board.beansInPlay(SOUTH)){
        over = true;
        hasWinner = false;
        return;
    }
    
}
    //If the game is over (i.e., the move member function has been called and returned false), 
    //set over to true; otherwise, set over to false and do not change anything else. 
    //If the game is over, set hasWinner to true if the game has a winner, or false if it resulted in a tie. 
    //If hasWinner is set to false, leave winner unchanged; otherwise, set it to the winning side.

bool Game::move(Side s)
{
    bool over = false;
    bool hasWinner = false;
    Side winner = NORTH;
    status(over, hasWinner, winner);
    if(over){
        // when game is over move all beans into each sides pot
        if(m_board.beansInPlay(SOUTH) == 0 || m_board.beansInPlay(NORTH) == 0)
        {
            for(int i = 1; i <= m_board.holes(); i++)
            {
                m_board.moveToPot(NORTH, i, NORTH);
                m_board.moveToPot(SOUTH, i, SOUTH);
            }
        }
        return false;
    }
    // While the moves made are valid and it is one sides turn, make a move
    while(true){
        int move = -1;
        if(s == SOUTH){
            move = m_south->chooseMove(m_board, SOUTH);
        } else if(s == NORTH){
            move = m_north->chooseMove(m_board,NORTH);
        }
        // If a move made was invalid due to game being over break out of loop
        if(move == -1) break;
        Side endSide;
        int endHole;
        if(!m_board.sow(s,move,endSide,endHole)) exit(-1);
        if(endSide == s){
            if(endHole == 0){  // If the endside of the move made was a players own pot make another move
                status(over,hasWinner,winner);
                if(over) break; //If landing in the sides own pot resulted in game over leave loop
                display(); // If not display screen and prompt same side to make another move
                if(s == NORTH){
                    cout << "Make another move!" << m_north->name() << endl;
                }
                if(s == SOUTH){
                    cout << "Make another move!" << m_south->name() << endl;
                }
                continue;
            }
            //If the end hole now has 1 beans and the opposite side hole has more than one, capture
            if(s == endSide && m_board.beans(endSide,endHole) == 1 && m_board.beans(opponent(endSide),endHole) > 0){
                m_board.moveToPot(s, endHole, s);
                m_board.moveToPot(opponent(s), endHole, s);
            }
            break;
        }
        else { break; } // endside was on opponents side
    }
    status(over,hasWinner,winner);
    bool beansLeft = m_board.beansInPlay(NORTH) > 0 || m_board.beansInPlay(SOUTH) > 0;
    if(over && beansLeft){
        display();
        if(m_board.beansInPlay(NORTH) > 0){
            cout << " Putting remaining beans into " << m_north->name() << "'s pot!" << endl;
        }
        else{
            cout << " Putting remaining beans into " << m_south->name() << "'s pot!" << endl; 
        }
        for(int i = 1; i <= m_board.holes(); i++){
                m_board.moveToPot(NORTH,i,NORTH);
                m_board.moveToPot(SOUTH,i,SOUTH);
        } 
    }
    return true;
}
    //Attempt to make a complete move for the player playing side s. "Complete" means that the player sows the seeds 
    //from a hole and takes any additional turns required or completes a capture. 
    //Whenever the player gets an additional turn, you should display the board so someone looking at the screen 
    //can follow what's happening. If the move can be completed, return true; if not, because the move is not yet 
    //completed but side s has no holes with beans to pick up and sow, sweep any beans in s's opponent's holes into that
    //opponent's pot and return false.

void Game::play()
{
    bool hasWinner = false;
    bool over = false;
    Side winner;
    status(over,hasWinner,winner);
    if(m_board.beansInPlay(SOUTH) == 0 || m_board.beansInPlay(NORTH) == 0 ){ // If game is over, sweep
         for(int i = 1; i < m_board.holes(); i++){
                m_board.moveToPot(NORTH,i,NORTH);
                m_board.moveToPot(SOUTH,i,SOUTH);
        } 
    }
    bool pressEnter = false;
    if(!m_north->isInteractive() && !m_south->isInteractive()){ // when both are interactive you must press enter
        pressEnter = true;
    }
    display();
    Side currentPlayer = SOUTH; // Start with South's turn
    while (!over) {
        move(currentPlayer);
        display();
        status(over, hasWinner, winner);
        if (pressEnter) {
            std::cout << "Press ENTER to continue";
            std::cin.ignore();
        }
        currentPlayer = opponent(currentPlayer); // Switch turns after each
    } 
    if(hasWinner){
        if(winner == NORTH)
        {
            cout << "The winner is "<< m_north->name() << endl;
        }
        else
        {
            cout << "The winner is "<< m_south->name() << endl;
        }
        
    }
    else if (!hasWinner)
    {
        cout << "The game resulted in a tie." << endl;
    }
}
    //Play the game. Display the progress of the game in a manner of your choosing, provided that someone looking 
    //at the screen can follow what's happening. If neither player is interactive, then to keep the display from 
    //quickly scrolling through the whole game, it would be reasonable periodically to prompt the viewer to press
    // ENTER to continue and not proceed until ENTER is pressed. (The ignore function for input streams is useful here.) 
    // Announce the winner at the end of the game. You can apportion to your liking the responsibility for displaying 
    //the board between this function and the move function.

int Game::beans(Side s, int hole) const
{
    return m_board.beans(s,hole);
}

//Return the number of beans in the indicated hole or pot of the game's board, or −1 if the hole number is invalid. 
//This function exists so that we and you can more easily test your program.