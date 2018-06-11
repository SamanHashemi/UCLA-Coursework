//
//  Tokenizer.hpp
//  Project4
//
//  Created by Saman Hashemipour on 3/11/18.
//  Copyright © 2018 Saman Hashemipour. All rights reserved.
//

#ifndef Tokenizer_hpp
#define Tokenizer_hpp

#include <stdio.h>
#include <string>


class Tokenizer {
public:
    Tokenizer(std::string separators);
    std::vector<std::string> tokenize(const std::string& s) const;
    
private:
    char *sepChars; // the character array that holds the separators
    int numSeps; // the number of separator values
};

#endif /* Tokenizer_hpp */
