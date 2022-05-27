#include "Language.hpp"
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "Traverser.hpp"
#include <fstream>
using namespace std;

////////////////////////////////////////////////

Language::Language(size_t tkStatesCapacity)
	: _unusedRuleId(1),
	  _unusedTkType(TKTYPE_EOF-1)
{
}

////////////////////////////////////////////////

string stringFromString(const string &s)
{
	return s;
}

void Language::setTkTypeRepr(TkType tkType, const string &repr)
{
	_parser.setTkTypeRepr(tkType, repr);
}

TkState Language::addWord(const string &word,
						  TkType resultType)
{
	_parser.setTkTypeRepr(resultType, word);
	return _tokenizer.addWord(word, resultType);
}

TkState Language::addPostStarWord(const string &word,
								  TkType resultType,
								  TkState fromState)
{
	string repr = "...";
	_parser.setTkTypeRepr(resultType, repr + word);
	return _tokenizer.addPostStarWord(word, resultType, fromState);
}

TkState Language::addCharStar(bool (*charFunc)(char),
							  TkType resultType,
							  TkState fromState)
{
	string repr = "?*";
	_parser.setTkTypeRepr(resultType, repr);
	_traverser.addLeafFunc(resultType, stringFromString);
	return _tokenizer.addStar(charFunc, resultType, fromState);
}

////////////////////////////////////////////////

void Language::setLogSuccess(bool logSuccess)
{
	_parser.setLogSuccess(logSuccess);
}

////////////////////////////////////////////////

// Comment line = one that starts with #
// (maybe preceded by whitespace)
bool langIsCommentLine(const string &line)
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

Value Language::interpret(const string &code)
{
	vector<Token> tokens;
    Token bofToken;
    bofToken._type = TKTYPE_BOF;
    tokens.push_back(bofToken);

	_tokenizer.tokenizeLine(tokens, code);
	
    Token eofToken;
    eofToken._type = TKTYPE_EOF;
    tokens.push_back(eofToken);

	auto tree = _parser.parse(tokens);
	return _traverser.traverse(tree);
}

void Language::tokenizeLine(vector<Token> &tokens, const string &line)
{
	_tokenizer.tokenizeLine(tokens, line);
}

Value Language::interpret(vector<Token> &tokens)
{
	auto tree = _parser.parse(tokens);
	return _traverser.traverse(tree);
}

////////////////////////////////////////////////
