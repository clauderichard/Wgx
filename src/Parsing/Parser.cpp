#include "Parser.hpp"
#include "Common/All.hpp"
using namespace std;

////////////////////////////////////////////////

ParserRule::ParserRule(
	PrRuleId id,
	TkType resultType,
	vector<TkType> stackTypes,
	vector<size_t> valuedChildIndexes)
	: _id(id),
	  _resultType(resultType),
	  _stackTypes(stackTypes),
	  _valuedChildIndexes(valuedChildIndexes) {}

ParserRule::~ParserRule() {}

TkType ParserRule::getTopType() const
{
	return _stackTypes[_stackTypes.size() - 1];
}

////////////////////////////////////////////////

ParseStack::ParseStack(size_t capacity)
{
	_treesStack.reserve(capacity);
}

size_t ParseStack::size() const
{
	return _treesStack.size();
}

ParseTree ParseStack::operator[](
	size_t index) const
{
	return _treesStack[index];
}

void ParseStack::pushToken(Token &token)
{
	_treesStack.emplace_back(token);
}

void ParseStack::reduce(const ParserRule &rule)
{
	size_t numTokens = rule._stackTypes.size();
	size_t firstStackI = _treesStack.size() - numTokens;

	ParseTree newTree(rule._id,
					  rule._resultType,
					  rule._valuedChildIndexes);
	for (size_t i = 0; i < numTokens; i++)
	{
		newTree.addChild(_treesStack[firstStackI + i]);
	}
	_treesStack.resize(firstStackI);
	_treesStack.push_back(newTree);
}

////////////////////////////////////////////////

ParserRuleTree::ParserRuleTree(int val)
	: _ruleIndex(val)
{
}
ParserRuleTree::~ParserRuleTree() {}

void ParserRuleTree::clear()
{
	_childMap.clear();
}

const ParserRuleTree *ParserRuleTree::getChild(TkType key) const
{
	return _childMap.at(key).get();
}

ParserRuleTree *ParserRuleTree::buildPath(TkType key)
{
	try
	{
		auto x = _childMap.at(key);
		return x.get();
	}
	catch (out_of_range)
	{
		_childMap[key] = shared_ptr<ParserRuleTree>(
			new ParserRuleTree(-1));
		auto y = _childMap.at(key);
		return y.get();
	}
}

void ParserRuleTree::addReversePath(
	const vector<TkType> pathKeys,
	int ruleIndex)
{
	ParserRuleTree *tree = this;
	for (auto it = pathKeys.crbegin(); it != pathKeys.crend(); it++)
	{
		tree = tree->buildPath(*it);
	}
	tree->_ruleIndex = ruleIndex;
}

////////////////////////////////////////////////

Parser::Parser()
	: _matchingTree(-1),
	  _rulesetInitialized(false),
	  _logSuccess(false)
{
	setTkTypeRepr(TKTYPE_EOF, "EOF");
}
Parser::~Parser() {}

void Parser::addRule(TkType resultType,
					 vector<TkType> stackTypes,
					 PrRuleId ruleId,
					 vector<size_t> valuedChildIndexes)
{
	_rules.emplace_back(ruleId, resultType, stackTypes, valuedChildIndexes);
	_rulesetInitialized = false;
}

void Parser::initialize()
{
	//cout << "Parser initializing" << endl;
	_matchingTree.clear();
	topToNextTrans();
	for (int rulI = 0; rulI < _rules.size(); rulI++)
	{
		auto rule = _rules[rulI];
		_matchingTree
			//.buildPath(TKTYPE_EOF)
			.addReversePath(rule._stackTypes, rulI);
		//for (auto nx : _topToNextTrans[rule._resultType])
		//		{
		//			_matchingTree.buildPath(nx)
		//				->addReversePath(rule._stackTypes, rulI);
		//		}
	}
	//cout << "Parser initialized!" << endl;
}

void Parser::topToNextTrans()
{
	TransMatrix transDownR(true); // maybe apply
	TransMatrix transSibR(false); // must be applied
	TransMatrix transUpL(true);	  // maybe apply
	for (auto rule : _rules)
	{
		transDownR.addArrow(rule._resultType, rule._stackTypes[0]);
		transUpL.addArrow(rule.getTopType(), rule._resultType);
		for (int i = 0; i < rule._stackTypes.size() - 1; i++)
		{
			transSibR.addArrow(rule._stackTypes[i], rule._stackTypes[i + 1]);
		}
	}
	transDownR.selfPowInf();
	transUpL.selfPowInf();
	_prevToBottomTrans = transSibR * transDownR;
	_topToNextTrans = transUpL * _prevToBottomTrans;
}

int Parser::getRuleMatch(
	const ParseStack &stackTokens,
	Token &nextInputTok) const
{
	int ret = -1;
	const ParserRuleTree *tree = &_matchingTree;

	try
	{
		//tree = tree->getChild(nextInputTok._type);
		for (size_t i1 = stackTokens.size(); i1 > 0; i1--)
		{
			tree = tree->getChild(stackTokens[i1 - 1].getToken()._type);
			if (tree->_ruleIndex >= 0 &&
				ruleWorksWithPreviousAndNext(
					stackTokens,
					_rules[tree->_ruleIndex],
					nextInputTok))
			{
				ret = tree->_ruleIndex;
			}
		}
	}
	catch (out_of_range &)
	{
	}

	return ret;
}

bool Parser::isNextTokenUnexpected(
	ParseStack &stackTokens,
	Token &nextInputTok)
{
	if (stackTokens.size() == 0)
	{
		return false;
	}
	TkType nextType = nextInputTok._type;
	if (nextType == TKTYPE_EOF)
	{
		return false;
	}
	TkType stackTopType = stackTokens[stackTokens.size() - 1].getToken()._type;
	return !_topToNextTrans.hasArrow(stackTopType, nextType);
}

bool Parser::ruleWorksWithPreviousAndNext(
	const ParseStack &stackTokens,
	const ParserRule &rule,
	const Token &nextToken) const
{
	size_t ruleSize = rule._stackTypes.size();

	TkType ruleResultType = rule._resultType;
	TkType nextType = nextToken._type;

	bool prevGood = true;
	if (stackTokens.size() == ruleSize)
	{
		prevGood = true;
	}
	else
	{
		TkType stackPrevType = stackTokens[stackTokens.size() - ruleSize - 1].getToken()._type;
		prevGood = _prevToBottomTrans.hasArrow(
			stackPrevType, ruleResultType);
	}
	return prevGood &&
		   (nextType == TKTYPE_EOF ||
			_topToNextTrans.hasArrow(ruleResultType, nextType));
}

ParseTree Parser::parse(vector<Token> &tokens)
{
	if (!_rulesetInitialized)
	{
		initialize();
		_rulesetInitialized = true;
	}

	// Input stream
	Token eofToken;
	eofToken._type = TKTYPE_EOF;
	tokens.push_back(eofToken);
	size_t nextInputIndex = 0;

	// stack
	ParseStack st(tokens.size());

	while (true)
	{
		if (nextInputIndex >= tokens.size())
		{
			// EOF passed, so Done!
			// expect [finalvalue,EOF]
			if (st.size() == 2)
			{
				if (_logSuccess)
				{
					cout << "Parser SUCCESS!!!" << endl;
				}
				return st[0];
			}
			else
			{
				cout << "Parser FAILED! Here's the stack:" << endl;
				for (size_t i = 0; i < st.size(); i++)
				{
					st[i].print(_tkTypeReprs, 2);
				}
				cout << endl;
				CRASH("Parser failed!");
			}
		}
		auto ruleMatch = getRuleMatch(st, tokens[nextInputIndex]);
		if (ruleMatch >= 0)
		{
			st.reduce(_rules[ruleMatch]);
		}
		else
		{
			st.pushToken(tokens[nextInputIndex++]);
			if (nextInputIndex < tokens.size() &&
				isNextTokenUnexpected(st, tokens[nextInputIndex]))
			{
				auto &badtok = tokens[nextInputIndex];
				cout << "Unexpected token of type ";
				cout << getTkTypeRepr(badtok._type);
				cout << endl
					 << "  Line number " << badtok._lineNumber;
				cout << endl
					 << "  Column number " << badtok._columnNumber;
				cout << endl
					 << "  Parsed stuff so far:"
					 << endl;
				for (size_t i = 0; i < st.size(); i++)
				{
					st[i].print(_tkTypeReprs, 2);
				}
				throw range_error("Unexpected token");
			}
		}
	}
}

string Parser::getTkTypeRepr(TkType tkType) const
{
	try
	{
		return _tkTypeReprs.at(tkType);
	}
	catch (const std::exception &)
	{
		return "?";
	}
}
void Parser::setLogSuccess(bool logSuccess)
{
	_logSuccess = logSuccess;
}
void Parser::setTkTypeRepr(TkType tkType, const string &repr)
{
	_tkTypeReprs[tkType] = repr;
}
void Parser::printTree(ParseTree tree, const string &code)
{
	tree.print(_tkTypeReprs, 2);
}

////////////////////////////////