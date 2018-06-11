#include "provided.h"
#include <string>
#include <vector>
using namespace std;

class TokenizerImpl
{
public:
    TokenizerImpl(string separators);
    vector<string> tokenize(const string& s) const;
private:
    bool isSeparator(char sep) const;
    vector<char> sepChars;
    int numSeps; // the number of separator values
    
};

TokenizerImpl::TokenizerImpl(string separators) // Should work, 99% certain
{
    for(int i = 0; i < separators.length(); i++){
        sepChars.push_back(separators[i]);
    }
}

vector<string> TokenizerImpl::tokenize(const string& s) const  // Should work, 99% certain
{
    std::vector<std::string> splitVals;
    std::string curString = "";
    for(int i = 0; i < s.length(); i++)
    {
        if(isSeparator(s[i])){
            if(curString.size() != 0)
            {
                splitVals.push_back(curString);
                curString = "";
            }
        }
        else{
            curString += s[i];
        }
    }
    return splitVals;
}


bool TokenizerImpl::isSeparator(char sep) const{
    for(int i = 0; i < sepChars.size(); i++){
        if(sepChars[i] == sep)
            return true;
    }
    return false;
}


//******************** Tokenizer functions ************************************

// These functions simply delegate to TokenizerImpl's functions.
// You probably don't want to change any of this code.

Tokenizer::Tokenizer(string separators)
{
    m_impl = new TokenizerImpl(separators);
}

Tokenizer::~Tokenizer()
{
    delete m_impl;
}

vector<string> Tokenizer::tokenize(const string& s) const
{
    return m_impl->tokenize(s);
}
