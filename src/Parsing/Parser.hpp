#ifndef __Parsing_PARSER_HPP__
#define __Parsing_PARSER_HPP__

#include "ParseTree.hpp"
#include "TransMatrix.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
using namespace std;

////////////////////////////////////////////////

struct ParserRule
{
	size_t _id;
	TkType _resultType;
	vector<TkType> _stackTypes;
	vector<size_t> _valuedChildIndexes;

  public:
	ParserRule(PrRuleId id,
			   TkType resultType,
			   vector<TkType> stackTypes,
			   vector<size_t> valuedChildIndexes);
	~ParserRule();

	TkType getTopType() const;
};

////////////////////////////////////////////////

class ParseStack
{
  public:
	ParseStack(size_t capacity);

	size_t size() const;

	ParseTree operator[](size_t index) const;

	void pushToken(Token &token);
	
	void reduce(const ParserRule &rule);

  private:
	vector<ParseTree> _treesStack;
};

////////////////////////////////////////////////

struct ParserRuleTree
{
	int _ruleIndex;
	unordered_map<TkType, shared_ptr<ParserRuleTree>> _childMap;

  public:
	ParserRuleTree(int ruleIndex);
	~ParserRuleTree();

	void clear();

	const ParserRuleTree *getChild(TkType key) const;

	ParserRuleTree *buildPath(TkType key);

	void addReversePath(const vector<TkType> pathKeys,
						int ruleIndex);
};

////////////////////////////////////////////////

class Parser
{
  public:
	Parser();
	~Parser();

	void addRule(
		TkType resultType,
		vector<TkType> stackTypes,
		PrRuleId ruleId,
		vector<size_t> valuedChildIndexes);

	ParseTree parse(vector<Token> &tokens);

	void setTkTypeRepr(TkType tkType, const string &repr);
	void setLogSuccess(bool logSuccess);
	void printTree(ParseTree tree, const string &code);

  private:
    void topToNextTrans();
	void initialize();
	bool isNextTokenUnexpected();
	bool ruleWorksWithPreviousAndNext(
		const ParserRule &rule) const;
	int getRuleMatch() const;
	TkType getNextTkType() const;

	string getTkTypeRepr(TkType tkType) const;

	vector<ParserRule> _rules;
	ParserRuleTree _matchingTree;
	TransMatrix _topToNextTrans;
	TransMatrix _prevToBottomTrans;

	bool _rulesetInitialized;

	map<TkType,string> _tkTypeReprs;
	bool _logSuccess;

	// Parsing state stuff
	vector<Token> *_inputTokens;
	ParseStack _stackTokens;
	size_t _nextInputIndex;

};

////////////////////////////////////////////////

#endif