#include "MyHash.h"
#include "provided.h"
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <fstream>

using namespace std;

class WordListImpl
{
public:
    bool loadWordList(string filename);
    bool contains(string word) const;
    vector<string> findCandidates(string cipherWord, string currTranslation) const;
private:
    MyHash<std::string, std::vector<std::string>> wordList;
//    Tokenizer t;
    std::string keyUp(std::string s) const;
};

bool WordListImpl::loadWordList(string filename)
{
    // TODO: Check if the file is opened correctly
    wordList.reset();
    bool addToList = true;
    string curLine = "";
    string lowerString = "";
    vector<string> curWords;
    int counter = 0;
    ifstream infile(filename);
    if (!infile)
        return false;
    while(getline(infile, curLine)){
        counter++;
        for(int i = 0; i < curLine.length(); i++){
            // If the current chacter is not a letter or apostrophe
            if(!isalpha(curLine[i]) && curLine[i] != '\''){
                addToList = false; // Dont add anything
                break; // move on to the next line
            }
            lowerString += tolower(curLine[i]); // add the uppercase letter to string
        }
        if(addToList){ // if line was valid
            if(wordList.find(keyUp(lowerString)) == nullptr){
                vector<string> newList;
                newList.push_back(lowerString);
                wordList.associate(keyUp(lowerString), newList);
            }
            else{
                wordList.find(keyUp(lowerString))->push_back(lowerString); // Put string in the vector
            }
        }
        lowerString = "";
        addToList = true;
    }
    return true;
}

bool WordListImpl::contains(string word) const // Should work 90% sure
{
    if((wordList.find(keyUp(word))) == nullptr){
        return false;
    }
    vector<string> words = *(wordList.find(keyUp(word)));
    for(int i = 0; i < words.size(); i++){
        if(words[i].compare(word) == 0){
            return true;
        }
    }
    return false;
}

vector<string> WordListImpl::findCandidates(string cipherWord, string currTranslation) const
{
    vector<string> candiates;
    if(wordList.find((keyUp(cipherWord))) == nullptr) // If the pointer is null then there is nothing corresponding w this string
        return candiates;
    int numMatching = 0; // The number of the matching values
    vector<string> possibleWords = *wordList.find((keyUp(cipherWord)));
    
    for(int i = 0; i < possibleWords.size(); i++){
        string compareWord = possibleWords[i];
        if(compareWord.size() == currTranslation.size()){
            for(int i = 0; i < compareWord.size(); i++){
                if(tolower(compareWord[i]) == tolower(currTranslation[i]) || currTranslation[i] == '?'){
                    numMatching++;
                }
                if(numMatching == compareWord.size()){
                    candiates.push_back(compareWord);
                }
            }
        }
        numMatching = 0;
    }
    
    return candiates;
}

std::string WordListImpl::keyUp(std::string s) const{

    MyHash<char, char> list;
    std::string tempString = "";
    int counter = 0;
    for(int i = 0; i < s.size(); i++){
        if(list.find(tolower(s[i])) == nullptr){
            list.associate(tolower(s[i]), counter + 'a');
            tempString += list.find(tolower(s[i]));
            counter++;
        }
        else{
            tempString += (list.find(tolower(s[i])));
        }
    }
    return tempString;

}

//***** hash functions for string, int, and char *****

unsigned int hash(const std::string& s)
{
    return std::hash<std::string>()(s);
}

unsigned int hash(const int& i)
{
    return std::hash<int>()(i);
}

unsigned int hash(const char& c)
{
    return std::hash<char>()(c);
}

//******************** WordList functions ************************************

// These functions simply delegate to WordListImpl's functions.
// You probably don't want to change any of this code.

WordList::WordList()
{
    m_impl = new WordListImpl;
}

WordList::~WordList()
{
    delete m_impl;
}

bool WordList::loadWordList(string filename)
{
    return m_impl->loadWordList(filename);
}

bool WordList::contains(string word) const
{
    return m_impl->contains(word);
}

vector<string> WordList::findCandidates(string cipherWord, string currTranslation) const
{
   return m_impl->findCandidates(cipherWord, currTranslation);
}
