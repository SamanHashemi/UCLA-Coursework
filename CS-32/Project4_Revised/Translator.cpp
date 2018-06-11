#include "MyHash.h"
#include "provided.h"
#include <string>
using namespace std;

class TranslatorImpl
{
public:
    TranslatorImpl();
    ~TranslatorImpl();
    bool pushMapping(string ciphertext, string plaintext);
    bool popMapping();
    string getTranslation(const string& ciphertext) const;
private:
    struct Node{
        MyHash<char, char> ciphToPlain;
        MyHash<char, char> plainToCiph;
        Node* next;
    };
    Node *m_top = nullptr;
    int popCount;
    int pushCount;
};

TranslatorImpl::TranslatorImpl(){
    popCount = 0;
    pushCount = 0;
    m_top = new Node;
//    m_top->ciphToPlain = new MyHash<char, char>;
//    m_top->plainToCiph = new MyHash<char, char>;
    m_top->next = nullptr;
    
}

TranslatorImpl::~TranslatorImpl() {

    while (m_top != nullptr) {
        Node * temp = m_top;
        m_top = m_top->next;
        delete temp;
    }
}

bool TranslatorImpl::pushMapping(string ciphertext, string plaintext)
{
    Node* curTable = new Node;
//    curTable->ciphToPlain = new MyHash<char, char>;
//    curTable->plainToCiph = new MyHash<char, char>;
    curTable->next = nullptr;
    
    // If the two strings have different sizes then they can't be the same
    if(ciphertext.size() != plaintext.size()){
        return false;
    }
    for(int i = 0; i < ciphertext.size(); i++){
        char cipher_char = ciphertext[i]; // cur ciphertext character
        char plain_char = plaintext[i]; // cur plaintext character
        
        // If either char is not a letter
        if((!isalpha(cipher_char) || !isalpha(plain_char)) && ((cipher_char != 39) || (plain_char != 39))){
//            delete curTable->ciphToPlain;
//            delete curTable->plainToCiph;
            delete curTable;
            return false;
        }
        
        // If the value mapped to is not a '?' or the table Value doesn't match the plaintext
        if(m_top->ciphToPlain.find(tolower(cipher_char)) != nullptr && *(m_top->ciphToPlain.find(tolower(cipher_char))) != '?' && tolower(*(m_top->ciphToPlain.find(tolower(cipher_char)))) != tolower(plain_char)){
            delete curTable;
            return false; //TODO: Check conditionals (below too)
        }
        
        // This alrady maps t
        if((m_top->plainToCiph.find(tolower(plain_char))) != nullptr && tolower(*(m_top->plainToCiph.find(tolower(plain_char)))) != tolower(cipher_char)){ // the vallue as already been associated w something
            delete curTable;
            return false;
        }

        curTable->ciphToPlain.associate(tolower(ciphertext[i]), tolower(plaintext[i]));
        curTable->plainToCiph.associate(tolower(plaintext[i]), tolower(ciphertext[i])); // map plaintext
    }
    
    for(int i = 0; i < 26; i++){
        // if there is nothing at that mapping and the cipher text matches the key
        if(m_top->ciphToPlain.find(i+'a') != nullptr && *m_top->ciphToPlain.find(i+'a') != '?'){
            curTable->ciphToPlain.associate(i+'a', *m_top->ciphToPlain.find(i+'a'));
        }
        else if(curTable->ciphToPlain.find(i+'a') == nullptr){
            curTable->ciphToPlain.associate(i+'a', '?');
        }
        if(m_top->plainToCiph.find(i+'a') != nullptr){
            curTable->plainToCiph.associate(i+'a', *m_top->plainToCiph.find(i+'a'));
        }
    }

    curTable->next = m_top;
    m_top = curTable;
    
    pushCount++; // Keep track of the number of times this function is called
    return true;
}

bool TranslatorImpl::popMapping()
{
    if(popCount >= pushCount){
        return false;
    }
    if(m_top == nullptr){
        return false;
    }
    
    Node* temp = m_top;
    m_top = m_top->next;
    delete temp;
    popCount++;
    return true;  // This compiles, but may not be correct
}

string TranslatorImpl::getTranslation(const string& ciphertext) const
{
    string returnString = "";
    for(int i = 0; i < ciphertext.size(); i++){
        
        if(m_top->ciphToPlain.find(tolower(ciphertext[i])) != nullptr){
            if(islower(ciphertext[i]))
                returnString += tolower(*m_top->ciphToPlain.find(tolower(ciphertext[i])));
            else if(isupper(ciphertext[i]))
                returnString += toupper(*m_top->ciphToPlain.find(tolower(ciphertext[i])));
            else
                returnString += *m_top->ciphToPlain.find(tolower(ciphertext[i]));
        }
        else if(isalpha(ciphertext[i])){
            returnString += '?';
        }
        else{
            returnString += ciphertext[i];
        }
        
    }
    return returnString;
}

//******************** Translator functions ************************************

// These functions simply delegate to TranslatorImpl's functions.
// You probably don't want to change any of this code.

Translator::Translator()
{
    m_impl = new TranslatorImpl;
}

Translator::~Translator()
{
    delete m_impl;
}

bool Translator::pushMapping(string ciphertext, string plaintext)
{
    return m_impl->pushMapping(ciphertext, plaintext);
}

bool Translator::popMapping()
{
    return m_impl->popMapping();
}

string Translator::getTranslation(const string& ciphertext) const
{
    return m_impl->getTranslation(ciphertext);
}
