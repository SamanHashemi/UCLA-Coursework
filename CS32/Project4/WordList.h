//
//  WordList.hpp
//  Project4
//
//  Created by Saman Hashemipour on 3/11/18.
//  Copyright © 2018 Saman Hashemipour. All rights reserved.
//

#ifndef WordList_hpp
#define WordList_hpp

#include <stdio.h>
#include <string>
#include "MyHash.h"


class WordList {
    
public:
    WordList();
    bool loadWordList(std::string filename);
    bool contains(std::string word) const;
    std::vector<std::string> findCandidates(std::string cipherWord, std::string currTranslation) const;
    
private:
    MyHash<int, std::vector<std::string>> wordList;
    int keyUp(std::string s) const;
};

#endif /* WordList_hpp */
