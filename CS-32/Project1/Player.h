//
//  player.h
//  CS32 Project 1
//
//  Created by Saman Hashemipour on 1/15/18.
//  Copyright © 2018 Saman Hashemipour. All rights reserved.
//

#ifndef player_h
#define player_h

#include <iostream>
//#include <string>
//#include <random>
//#include <utility>
//#include <cstdlib>
//


#include "City.h"
//#include "globals.h"

class City;

class Player
{
public:
    // Constructor
    Player(City *cp, int r, int c);
    
    // Accessors
    int  row() const;
    int  col() const;
    int  age() const;
    int  health() const;
    bool isPassedOut() const;
    
    // Mutators
    void  preach();
    void  move(int dir);
    void  getGassed();
    
private:
    City* m_city;
    int   m_row;
    int   m_col;
    int   m_health;
    int   m_age;
};

#endif /* player_h */
