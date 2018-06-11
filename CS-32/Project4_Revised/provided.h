#ifndef PROVIDED_INCLUDED
#define PROVIDED_INCLUDED

#include <string>
#include <vector>

class TokenizerImpl;

class Tokenizer
{
public:
    Tokenizer(std::string separators);
    ~Tokenizer();
    std::vector<std::string> tokenize(const std::string& s) const;
      // We prevent a Tokenizer object from being copied or assigned.
    Tokenizer(const Tokenizer&) = delete;
    Tokenizer& operator=(const Tokenizer&) = delete;
private:
    TokenizerImpl* m_impl;
};

class WordListImpl;

class WordList
{
public:
    WordList();
    ~WordList();
    bool loadWordList(std::string filename);
    bool contains(std::string word) const;
    std::vector<std::string> findCandidates(std::string cipherWord, std::string currTranslation) const;
      // We prevent a WordList object from being copied or assigned.
    WordList(const WordList&) = delete;
    WordList& operator=(const WordList&) = delete;
private:
    WordListImpl* m_impl;
};

class TranslatorImpl;

class Translator
{
public:
    Translator();
    ~Translator();
    bool pushMapping(std::string ciphertext, std::string plaintext);
    bool popMapping();
    std::string getTranslation(const std::string& ciphertext) const;
      // We prevent an Translator object from being copied or assigned.
    Translator(const Translator&) = delete;
    Translator& operator=(const Translator&) = delete;
private:
    TranslatorImpl* m_impl;
};

class DecrypterImpl;

class Decrypter
{
public:
    Decrypter();
    ~Decrypter();
    bool load(std::string filename);
    std::vector<std::string> crack(const std::string& ciphertext);
      // We prevent a Decrypter object from being copied or assigned.
    Decrypter(const Decrypter&) = delete;
    Decrypter& operator=(const Decrypter&) = delete;
private:
    DecrypterImpl* m_impl;
};

#endif // PROVIDED_INCLUDED
