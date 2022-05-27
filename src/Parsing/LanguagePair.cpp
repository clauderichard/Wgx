#include "LanguagePair.hpp"
#include <fstream>
using namespace std;

LanguagePair::LanguagePair(
    Language *lang1, Language *lang2,
    bool (*lineIsDelimiter)(const string &),
    void (*executeBetweenLanguages)(Value &value1))
    : _lang1(lang1),
      _lang2(lang2),
      _lineIsDelimiter(lineIsDelimiter),
      _executeBetweenLanguages(executeBetweenLanguages) {}
      
pair<Value,Value> LanguagePair::interpretFile(const string &filepath)
{
    auto tokenss = tokenizeFile(filepath);
    auto value1 = _lang1->interpret(tokenss.first);
    _executeBetweenLanguages(value1);
    auto value2 = _lang2->interpret(tokenss.second);
    return pair<Value,Value>(value1, value2);
}

bool langpairIsCommentLine(const string &line)
{
	for (auto c : line)
	{
		if (c == '#')
		{
			return true;
		}
		if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
		{
			return false;
		}
	}
	return false;
}
pair<vector<Token>,vector<Token>> LanguagePair::tokenizeFile(
    const string &filepath)
{
    pair<vector<Token>,vector<Token>> ret;
    Token bofToken;
    bofToken._type = TKTYPE_BOF;
    ret.first.push_back(bofToken);
    ret.second.push_back(bofToken);
    bool useLang1 = true;
    
	ifstream myfile(filepath);
	string line;
	if (myfile.is_open())
	{
		//bool newl = false;
		while (getline(myfile, line))
		{
            if (_lineIsDelimiter(line))
            {
                useLang1 = false;
                // useLang = _lang2;
                // useTokens = &ret.second;
            }
			else if (!langpairIsCommentLine(line))
			{
                if (useLang1)
                {
				    _lang1->tokenizeLine(ret.first, line);
                }
                else
                {
				    _lang2->tokenizeLine(ret.second, line);
                }
			}
		}
	}
	else
	{
		throw range_error("File not found");
	}
    Token eofToken;
    eofToken._type = TKTYPE_EOF;
    ret.first.push_back(eofToken);
    ret.second.push_back(eofToken);
    return ret;
}
