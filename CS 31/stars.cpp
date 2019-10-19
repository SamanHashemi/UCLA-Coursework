//
//  main.cpp
//  Project 5
//
//  Created by Saman Hashemipour on 11/18/17.
//  Copyright © 2017 Saman Hashemipour. All rights reserved.
//

#include <iostream>
#include "utilities.h"

using namespace std;

int runOneRound(const char words[][7], int nWords, int wordnum);

int main(int argc, const char * argv[]) {
    const int MAXWORDLEN = 6;
    const int MAXNUMWORDS = 9000;
    const char WORDFILENAME[] = "/Users/Saman-Mac/Desktop/School/Fall 2017/CS 31/Project 2/Project 2/words.txt";
    
    
    
    char wordList[MAXNUMWORDS][MAXWORDLEN+1];
    int nWords = getWords(wordList, MAXNUMWORDS, WORDFILENAME);
    int nRound;
    
    // Must return more than 1 words
    if(nWords < 1){
        cout << "No words were loaded, so I can't play the game.";
        return 0;
    }
    
    int maxRounds = 0;
    int minRounds = 0;
    int numRounds = 0;
    double totalRounds = 0;
    double aveRounds = 0.0;
    
    cout << "How many rounds do you want to play? ";
    cin >> nRound;
    
    if(nRound < 0){
        cout << "The number of rounds must be positive";
        return 0;
    }
    
    for(int i = 1; i <= nRound; i++){
        cout << endl;
        cout << "Round " << i << endl;
        int wordnum = randInt(0, nWords-1); // Create the random word number
        cout << "The secret word is " << strlen(wordList[wordnum]) << " letters long." << endl; // Get word size
        
        numRounds = runOneRound(wordList, nWords, wordnum); // Play round of guessing
        
        // Print the numer of rounds it took to play
        if(numRounds == 1){
            cout << "You got it in 1 try." << endl;
        }
        else{
            cout << "You got it in " << numRounds << " tries." << endl;
        }
        
        totalRounds += numRounds; // The total number of rounds played so far
        aveRounds = totalRounds/i; // The ave number of plays so far
        
        //Find the min and max rounds played
        if(numRounds > maxRounds)
            maxRounds = numRounds;
        if(numRounds < minRounds || minRounds == 0)
            minRounds = numRounds;
        
        // Print out the relevant info
        cout.setf(ios::fixed);
        cout.precision(2);
        cout << "Average: " << aveRounds << ", ";
        cout << "minimum: " << minRounds << ", ";
        cout << "maximum: " << maxRounds << endl;
        
        cout << endl << endl;
        
    }
}

int runOneRound(const char words[][7], int nWords, int wordnum){
    // First test case for wordnum and nWords requirements (LOOOOOOOOOOLLLLLLLLL CHECK THIS SHITTTTTT LOOOOOOOOOOOOOLLLLLLLLLLL)
    if(wordnum >= nWords || wordnum < 0 || nWords < 0){
        return -1;
    }
    
    int planets = 0; // Number of correct letters
    int stars = 0; // Number of correct locations
    
    int numTries = 0;
    
    bool inList = false;
    bool lowerCase = true;
    bool wordFound = false; // Was the word guessed correctly
    char word[MAXWORDLEN+1]; // The word that will be used
    char probeWord[101]; // The word that the person will use
    
    // Create and initialize the array for the locations of the string that have been counted
    bool beenCounted[MAXWORDLEN+1];
    for(int i = 0; i < MAXWORDLEN+1; i++){
        beenCounted[i] = false;
    }
    
    strcpy(word, words[wordnum]);
    
//    cout << word << endl; // To cheat
    
    while(!wordFound){
        cout << "Probe word: ";
        cin.getline(probeWord, 100);
        if(probeWord[0] == 0)
            continue;
        
        //If not lower case and 4,5 or 6 letters long
        lowerCase = true;
        for(int i = 0; i < strlen(probeWord); i++){
            if(isupper(probeWord[i])){
                lowerCase = false;
            }
        }
        if(!lowerCase || (strlen(probeWord) < 4 || strlen(probeWord) > 6)){
            cout << "Your probe word must be a word of 4 to 6 lower case letters." << endl;
            continue;
        }
        
        // Check to see if in the list of words
        inList = false;
        for(int i = 0; i < nWords; i++){
            if(strcmp(probeWord, words[i]) == 0){
                inList = true;
            }
        }
        if(!inList){
            cout << "I don't know that word" << endl;
            continue;
        }
        
        numTries++; // If made it passed the tests increase number of guesses
        
        // Look for stars
//        for(int i = 0; i < strlen(word); i++){
//            if(i >= strlen(probeWord))
//                break;
//            if(probeWord[i] == word[i]){
//                stars++;
//                beenCounted[i] = true;
//            }
//        }
        
        
        
        // Look for planets
        for(int j = 0; j < strlen(probeWord); j++){
            for(int i = 0; i < strlen(word); i++){
                // Check for a star
                if(!beenCounted[i] && probeWord[j] == word[i] && i == j){
                    stars++;
                    beenCounted[i] = true;
                    break;
                }
                // Check for a planet
                if(!beenCounted[i] && probeWord[j] == word[i]){
                    planets++;
                    beenCounted[i] = true;
                    break;
                }
            }
        }
        
        // Guessed the right word
        if(stars == strlen(word) && strlen(probeWord) == strlen(word)){
            return numTries;
        }
        
        // Print planet and stars
        cout << "Stars: " << stars << ", " << "Planets: " << planets << endl;
        
        //Reset all values to prepare for the next word
        for(int i = 0; i < MAXWORDLEN+1; i++){
            beenCounted[i] = false;
        }
        stars = 0;
        planets = 0;
    }
    return numTries;
}



