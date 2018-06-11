//
//  main.cpp
//  Project4
//
//  Created by Saman Hashemipour on 3/11/18.
//  Copyright © 2018 Saman Hashemipour. All rights reserved.
//

#include <iostream>
#include <string>
#include "MyHash.h"
using namespace std;

int main() {
    MyHash<string, string> testing;
    string t = "";
    string toRef = "20";
    string toKey = "5";
    for(int i = 0; i < 200; i++)
    {
        toKey = to_string(i);
        toRef = to_string(i*2);
        testing.associate(toKey, toRef);
        if(testing.find(toRef) == nullptr){
            std::cout << "fuck a null pointer bruh";
        }
        else{
            t = *testing.find(toRef);
        }
        std::cout << t;
    }
}
