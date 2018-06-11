//
//  city.h
//  CS32 Project 1
//
//  Created by Saman Hashemipour on 1/15/18.
//  Copyright © 2018 Saman Hashemipour. All rights reserved.
//

#ifndef city_h
#define city_h

#include <iostream>
#include <string>
#include <random>
#include <utility>
#include <cstdlib>

#include "History.h"
#include "globals.h"

class Player;
class Flatulan;
class History;


class City
{
public:
    // Constructor/destructor
    City(int nRows, int nCols);
    ~City();
    
    // Accessors
    int      rows() const;
    int      cols() const;
    Player*  player() const;
    History& history();
    int      flatulanCount() const;
    int      nFlatulansAt(int r, int c) const;
    bool     determineNewPosition(int& r, int& c, int dir) const;
    void     display() const;
    
    // Mutators
    bool  addFlatulan(int r, int c);
    bool  addPlayer(int r, int c);
    void  preachToFlatulansAroundPlayer();
    void  moveFlatulans();
    
private:
    int       m_rows;
    int       m_cols;
    Player*   m_player;
    Flatulan* m_flatulans[MAXFLATULANS];
    int       m_nFlatulans;
    History   m_history;
    
    // Helper functions
    bool isInBounds(int r, int c) const;
};


#endif /* city_h */
