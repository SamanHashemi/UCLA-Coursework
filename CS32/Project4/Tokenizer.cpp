//
//  Tokenizer.cpp
//  Project4
//
//  Created by Saman Hashemipour on 3/11/18.
//  Copyright © 2018 Saman Hashemipour. All rights reserved.
//

#include "Tokenizer.h"
#include <string>
#include <vector>

Tokenizer::Tokenizer(std::string separators){
    sepChars = new char[separators.length()];
    for(int i = 0; i < separators.length(); i++){
        sepChars[i] = separators[i];
        numSeps++;
    }
}

std::vector<std::string> Tokenizer::tokenize(const std::string& s) const{
    std::vector<std::string> splitVals;
    std::string curString = "";
    for(int i = 0; i < s.length(); i++){
        for(int j = 0; j < numSeps; j++){
            if(s[i] == sepChars[j]){
                splitVals.push_back(curString);
                curString = "";
                break;
            }
            else{
                curString += s[i];
            }
        }
    }
    return splitVals;
}
