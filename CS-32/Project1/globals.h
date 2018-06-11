//
//  global.h
//  CS32 Project 1
//
//  Created by Saman Hashemipour on 1/15/18.
//  Copyright © 2018 Saman Hashemipour. All rights reserved.
//

#ifndef global_h
#define global_h

const int MAXROWS = 20;               // max number of rows in a city
const int MAXCOLS = 30;               // max number of columns in a city
const int MAXFLATULANS = 100;         // max number of Flatulans allowed
const int INITIAL_PLAYER_HEALTH = 10;

const int UP      = 0;
const int DOWN    = 1;
const int LEFT    = 2;
const int RIGHT   = 3;
const int NUMDIRS = 4;

// Utilities implementation
int randInt(int min, int max);
void clearScreen();

#endif /* global_h */
