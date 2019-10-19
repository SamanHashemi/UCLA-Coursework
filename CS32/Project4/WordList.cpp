//
//  WordList.cpp
//  Project4
//
//  Created by Saman Hashemipour on 3/11/18.
//  Copyright © 2018 Saman Hashemipour. All rights reserved.
//

#include "WordList.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>


WordList::WordList(){
    
}

unsigned int hash(const std::string& s);
unsigned int hash(const int& i);
unsigned int hash(const char& s);

unsigned int hash(const std::string& s) {
    return std::hash<std::string>()(s);
}
unsigned int hash(const int& i){
    std::cout << "happin here";
    return std::hash<int>()(i);
}
unsigned int hash(const char& c){
    return std::hash<char>()(c);
}

bool WordList::loadWordList(std::string filename){
    wordList.reset();
    int key = 0;
    std::string lowerString = "";
    std::string curLine;
    std::ifstream myfile;
    std::vector<std::string> curWords;
    myfile.open(filename);
    if(myfile.is_open()){
        while(std::getline(myfile, curLine)){
            for(int i = 0; i < curLine.size(); i++){
                // If the current chacter is not a letter or apostrophe
                if(!isalpha(curLine[i]) && curLine[i] != 39){
                    break;
                }
                lowerString += tolower(curLine[i]);
            }
            key = keyUp(lowerString);
            curWords = *wordList.find(key);
            curWords.push_back(curLine);
            wordList.associate(key, curWords);
            key = 0;
        }
        return true;
    }
    return false;
}

bool WordList::contains(std::string word) const {
//    if((wordList.find(1)) == nullptr){
//        return false;
//    }
//    for(int i = 0; i < (*(wordList.find(keyUp(word)))).size(); i++){
//
//    }
//    std::vector<std::string> words;
//    for(int i = 0; i < words.size(); i++){
//        if(words[i].compare(word) == 0){
//            return true;
//        }
//    }
    return false;
}


std::vector<std::string> WordList::findCandidates(std::string cipherWord, std::string currTranslation) const{
    std::vector<std::string> candiates;
//    int numMatching = 0;
//    std::vector<std::string> possibleWords = *wordList.find((keyUp(cipherWord)));
//    for(int i = 0; i < possibleWords.size(); i++){
//        std::string compareWord = possibleWords[i];
//        if(compareWord.size() == currTranslation.size()){
//            for(int i = 0; i < possibleWords.size(); i++){
//                if(tolower(compareWord[i]) == tolower(currTranslation[i]) || currTranslation[i] == '?'){
//                    numMatching++;
//                    break;
//                }
//                if(numMatching == compareWord.size()){
//                    candiates.push_back(compareWord);
//                }
//            }
//        }
//        numMatching = 0;
//    }
    return candiates;
}


int WordList::keyUp(std::string s) const{
    //TODO: May need to make lower case in here

    // Create the counter and the new array to hold the repeat values
    int tempIntHolder[s.size()];
    int counter = 1;
    // Set int array to 0
    for(int i = 0; i < s.size(); i++){
        tempIntHolder[i] = 0;
    }
    // Go through the string and add the correct numbers to the array
    for(int i = 0; i < s.size(); i++){
        for(int j = i; j < s.size(); j++){
            if(s[i] == s[j] && i != j && tempIntHolder[j] == 0){
                tempIntHolder[j] = counter;
            }
        }
        tempIntHolder[i] = counter++;
    }
    s = " "; // make "+=" not "=" when this function returns a string
    // Add the integers to the string
    for(int i = 0; i < s.size(); i++){
        s += std::to_string(tempIntHolder[i]);
    }
    return std::stoi(s);
    return 0;
}






















