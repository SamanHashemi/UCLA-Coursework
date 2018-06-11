//
//  main.cpp
//  Project 5
//
//  Created by Saman Hashemipour on 11/18/17.
//  Copyright © 2017 Saman Hashemipour. All rights reserved.
//

#include <iostream>
#include "utilities.cpp"

int main(int argc, const char * argv[]) {
    const int MAXWORDLEN = 6;
    const char WORDFILENAME[] = "words.txt";
    
    char words[9000][MAXWORDLEN+1];
    int nWords = getWords(words, 9000, WORDFILENAME);
    
    int nRound;
    cout << "How many rounds do you want to play? ";
    cin >> nRound;
    
    for(int i = 1; i <= nRound; i++){
        bool wordFound = false;
        cout << "Round " << i << endl;
        string word = words[randInt(0,7265)];
        cout << word << endl;
        cout << "The secret word is " << word.size() << " letters long.";
        while(!wordFound){
            wordFound = true;
        }
    }
}
