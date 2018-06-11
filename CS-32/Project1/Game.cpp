
//#include <random>
//#include <utility>
//#include <cstdlib>
//
#include "Game.h"

int decodeDirection(char dir);

Game::Game(int rows, int cols, int nFlatulans)
{
    if (nFlatulans < 0)
    {
        std::cout << "***** Cannot create Game with negative number of Flatulans!" << std::endl;
        exit(1);
    }
    if (nFlatulans > MAXFLATULANS)
    {
        std::cout << "***** Trying to create Game with " << nFlatulans
        << " Flatulans; only " << MAXFLATULANS << " are allowed!" << std::endl;
        exit(1);
    }
    if (rows == 1  &&  cols == 1  &&  nFlatulans > 0)
    {
        std::cout << "***** Cannot create Game with nowhere to place the Flatulans!" << std::endl;
        exit(1);
    }
    
    // Create city
    m_city = new City(rows, cols);
    
    // Add player
    int rPlayer = randInt(1, rows);
    int cPlayer = randInt(1, cols);
    m_city->addPlayer(rPlayer, cPlayer);
    
    // Populate with Flatulans
    while (nFlatulans > 0)
    {
        int r = randInt(1, rows);
        int c = randInt(1, cols);
        // Don't put a Flatulan where the player is
        if (r == rPlayer  &&  c == cPlayer)
            continue;
        m_city->addFlatulan(r, c);
        nFlatulans--;
    }
}

Game::~Game()
{
    delete m_city;
}

void Game::play()
{
    m_city->display();
    Player* p = m_city->player();
    if (p == nullptr)
        return;
    
    while ( ! p->isPassedOut()  &&  m_city->flatulanCount() > 0)
    {
        std::cout << "Move (u/d/l/r/h//q): ";
        std::string action;
        getline(std::cin,action);
        if (action.size() == 0)  // player preaches
            p->preach();
        else
        {
            switch (action[0])
            {
                default:   // if bad move, nobody moves
                    std::cout << '\a' << std::endl;  // beep
                    continue;
                case 'q':
                    return;
                case 'h':
                    m_city->history().display();
                    std::cout << "Press Enter to continue";
                    std::cin.ignore(10000,'\n');
                    break;
                case 'u':
                case 'd':
                case 'l':
                case 'r':
                    p->move(decodeDirection(action[0]));
                    break;
            }
        }
        if(action[0] != 'h'){
            m_city->moveFlatulans();
            m_city->display();
        }
    }
    if (p->isPassedOut())
        std::cout << "You lose." << std::endl;
    else
        std::cout << "You win." << std::endl;
}


int decodeDirection(char dir)
{
    switch (dir)
    {
        case 'u':  return UP;
        case 'd':  return DOWN;
        case 'l':  return LEFT;
        case 'r':  return RIGHT;
    }
    return -1;  // bad argument passed in!
}
