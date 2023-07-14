#include <iostream>
#include "Board.h"

using namespace std;


Board::Board(int nHoles, int nInitialBeansPerHole) : m_nHoles(nHoles), m_nInitialBeansPerHole(nInitialBeansPerHole)
{
    if(nHoles <= 0) m_nHoles = 1; //invalid hole number
    if(nInitialBeansPerHole < 0) m_nInitialBeansPerHole = 0; //invalid bean number
    m_north.push_back(0); //north pot
    m_south.push_back(0); //south pot
    for(int i = 1; i <= m_nHoles; i++){ //adding specified beans to each hole
        m_north.push_back(m_nInitialBeansPerHole);
        m_south.push_back(m_nInitialBeansPerHole);
    }
}
//Construct a Board with the indicated number of holes per side (not counting the pot) and initial number of beans per hole. 
//If nHoles is not positive, act as if it were 1; if nInitialBeansPerHole is negative, act as if it were 0.
Board::Board(const Board& other) : m_north(other.m_north), m_south(other.m_south)
{
    m_nHoles = other.m_nHoles; 
    m_nInitialBeansPerHole = other.m_nInitialBeansPerHole;
}
//Custom copy constructor
int Board::holes() const
{
    return m_nHoles;
}
//Return the number of holes on a side (not counting the pot).
int Board::beans(Side s, int hole) const
{
    if(hole < 0 || hole > m_nHoles)
        return -1;
    if(s == NORTH) return m_north[hole];
    else return m_south[hole];
}
//Return the number of beans in the indicated hole or pot, or −1 if the hole number is invalid.
int Board::beansInPlay(Side s) const
{   
    int count = 0;
    for(int i = 1; i <= m_nHoles; i++){
        count += beans(s,i);
    }
    return count;
}
//Return the total number of beans in all the holes on the indicated side, not counting the beans in the pot.
int Board::totalBeans() const
{
    int count = 0;
    for(int i = 0; i <= m_nHoles; i++){
        count += m_north[i] + m_south[i];
    }
    return count;
}
//Return the total number of beans in the game, including any in the pots.
bool Board::sow(Side s, int hole, Side& endSide, int& endHole)
{
    if(hole <= 0 || hole > m_nHoles) // check if hole is valid
        return false;
    int count = 0;
    if(s == NORTH) // if start size is NORTH
    {
        count = m_north[hole]; // get ammount of beans at hole
        if(count == 0) // if zero return
            return false;
        m_north[hole] = 0; // set beans at hole to zero
        int pos = hole - 1; // start position for first loop is one hole to the left of chosen hole
        
        while(count != 0) // while loop for looping through the board
        {
            for(int i = pos; i >= 0 && count != 0; i--) // backwards while loop for north
            {
                m_north[i]++;
                count--;
                if(count == 0)
                {
                    endSide = NORTH;
                    endHole = i;
                    return true;
                }
            }
            pos = m_nHoles; // set the start for next loop to be the last hole
            for(int i = 1; i < m_south.size() ; i++) // loop through south from first hole to last
            {
                m_south[i]++;
                count--;
                if(count == 0)
                {
                    endSide = SOUTH;
                    endHole = i;
                    return true;
                }
            }
//            if(count < 0)
//                exit(4);
        }
        return true;
    }
    else // if start side is SOUTH
    {
        count = m_south[hole]; // make a count of how many beans
        if(count == 0)
            return false;
        m_south[hole] = 0; // make starting hole = 0
        
        int pos = hole + 1; // starting position for the first loop is one past the chosen hole
        
        while(count != 0) // while loop for looping through the board
        {
            for(int i = pos; i < m_south.size() && count != 0; i++) // loop through south
            {
                m_south[i]++;
                count--;
                if(count == 0)
                {
                    endSide = SOUTH;
                    endHole = i;
                    return true;
                }
            }
            pos = 1; // set starting position for next loop to one
            if(count != 0) // add to south's pot
            {
                m_south[0]++;
                count--;
            }
            if(count == 0)
            {
                endSide = SOUTH;
                endHole = 0;
                return true;
            }
            for(int i = m_nHoles; i > 0; i--) // loop backwards through north
            {
                m_north[i]++;
                count--;
                if(count == 0)
                {
                    endSide = NORTH;
                    endHole = i;
                    return true;
                }
            }
//            if(count < 0)
//                exit(4);
        }
        return true;
    }
}
//If the hole indicated by (s,hole) is empty or invalid or a pot, this function returns false without changing anything. 
//Otherwise, it will return true after sowing the beans: the beans are removed from hole (s,hole) and sown counterclockwise,
//including s's pot if encountered, but skipping s's opponent's pot. The function sets the parameters endSide and 
//endHole to the side and hole where the last bean was placed. 
bool Board::moveToPot(Side s, int hole, Side potOwner)
{
    if(hole > m_nHoles || hole <= 0) return false;
    int count = 0;
    if(s == NORTH){
        count = m_north[hole];
        m_north[hole] = 0;
    }
    else{
        count = m_south[hole];
        m_south[hole] = 0;
    }
    if(potOwner == NORTH)
    {
        m_north[0] += count; //add beans from hole to north pot
        return true;
    }
    else
    {
        m_south[0] += count; //add beans from hole to south pot
        return true;
    }
}
//If the indicated hole is invalid or a pot, return false without changing anything. 
//Otherwise, move all the beans in hole (s,hole) into the pot belonging to potOwner and return true.
bool Board::setBeans(Side s, int hole, int beans)
{
    if(hole > m_nHoles || hole < 0 || beans < 0) return false;
    if(s == NORTH){
        m_north[hole] = beans;
        return true;
    }else
    {
        m_south[hole] = beans;
        return true;
    }
}
//If the indicated hole is invalid or beans is negative, this function returns false without changing anything. 
//Otherwise, it will return true after setting the number of beans in the indicated hole or pot to the 
//value of the third parameter. (This could change what beansInPlay and totalBeans return if they are called later.) 
//This function exists solely so that we and you can more easily test your program: 
//None of your code that implements the member functions of any class is allowed to call this function directly or indirectly. (We'll show an example of its use below.)
