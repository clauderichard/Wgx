#ifndef __LANGUAGEPAIR_HPP__
#define __LANGUAGEPAIR_HPP__

#include "Language.hpp"
#include "Token.hpp"
#include <utility>
#include <string>
using namespace std;

class LanguagePair
{
  public:
	  LanguagePair(Language *lang1, Language *lang2, 
        bool (*lineIsDelimiter)(const string &),
        void (*executeBetweenLanguages)(Value &value1));
        
    pair<Value,Value> interpretFile(const string &filepath);
	
  private:
    pair<vector<Token>,vector<Token>>
    	tokenizeFile(const string &filepath);

    Language *_lang1;
    Language *_lang2;
    bool (*_lineIsDelimiter)(const string &);
    void (*_executeBetweenLanguages)(Value &value1);
};

#endif