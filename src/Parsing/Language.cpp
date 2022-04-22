#include "Language.hpp"
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "Traverser.hpp"
#include <fstream>
using namespace std;

////////////////////////////////////////////////

Language::Language(size_t tkStatesCapacity)
	: _unusedRuleId(1),
	  _unusedTkType(-11)
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

Value Language::interpretFile(const string &filepath)
{
	vector<Token> tokens;
	ifstream myfile(filepath);
	string line;
	//size_t sectionLineNumber = 1;
	//bool isInsideComment = false;
	if (myfile.is_open())
	{
		//bool newl = false;
		while (getline(myfile, line))
		{
			if (!langIsCommentLine(line))
			{
				_tokenizer.tokenizeLine(tokens,line);
			}
		}
	}
	else
	{
		throw range_error("File not found");
	}
	//_tokenizer.tokenizeLine(tokens, code);
	// auto tokens = _tokenizer.tokenize(code);
	auto tree = _parser.parse(tokens);
	return _traverser.traverse(tree);
}

Value Language::interpret(const string &code)
{
	vector<Token> tokens;
	_tokenizer.tokenizeLine(tokens, code);
	// auto tokens = _tokenizer.tokenize(code);
	auto tree = _parser.parse(tokens);
	return _traverser.traverse(tree);
}

////////////////////////////////////////////////
