//
//  game.h
//  CS32 Project 1
//
//  Created by Saman Hashemipour on 1/15/18.
//  Copyright © 2018 Saman Hashemipour. All rights reserved.
//

#ifndef game_h
#define game_h

#include <iostream>
#include <string>

#include "globals.h"
#include "Player.h"

class Game
{
public:
    // Constructor/destructor
    Game(int rows, int cols, int nFlatulans);
    ~Game();
    
    // Mutators
    void play();
    
private:
    City* m_city;
};

#endif /* game_h */
