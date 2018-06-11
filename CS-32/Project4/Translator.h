//
//  Translator.hpp
//  Project4
//
//  Created by Saman Hashemipour on 3/12/18.
//  Copyright © 2018 Saman Hashemipour. All rights reserved.
//

#ifndef Translator_hpp
#define Translator_hpp

#include <stdio.h>
#include <string>
#include "MyHash.h"

class Translator {
public:
    Translator();
    bool pushMapping(std::string ciphertext, std::string plaintext);
    bool popMapping();
    std::string getTranslation(const std::string& ciphertext) const;
private:
    struct Node{
        MyHash<char, char> *data;
        Node* below;
    };
    Node* top = nullptr;
    MyHash<char, char> *m_mapTable;
};

#endif /* Translator_hpp */
