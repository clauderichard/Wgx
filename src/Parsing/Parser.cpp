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
	  _logSuccess(false),
	  _inputTokens(NULL),
	  _stackTokens(1)
{
	setTkTypeRepr(TKTYPE_BOF, "BOF");
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
			.addReversePath(rule._stackTypes, rulI);
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

int Parser::getRuleMatch() const
{
	int ret = -1;
	const ParserRuleTree *tree = &_matchingTree;

	try
	{
		for (size_t i1 = _stackTokens.size(); i1 > 0; i1--)
		{
			tree = tree->getChild(_stackTokens[i1 - 1].getToken()._type);
			if (tree->_ruleIndex >= 0 &&
				ruleWorksWithPreviousAndNext(
					_rules[tree->_ruleIndex]))
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

bool Parser::isNextTokenUnexpected()
{
	if (_nextInputIndex >= _inputTokens->size())
	{
		return false;
	}
	else if (_stackTokens.size() == 0)
	{
		return false;
	}
	else
	{
		TkType nextType = getNextTkType();
		TkType stackTopType = _stackTokens[_stackTokens.size() - 1].getToken()._type;
		return !_topToNextTrans.hasArrow(stackTopType, nextType);
	}
}

bool Parser::ruleWorksWithPreviousAndNext(
	const ParserRule &rule) const
{
	size_t ruleSize = rule._stackTypes.size();
	TkType ruleResultType = rule._resultType;

	bool prevGood = true;
	bool nextGood = true;
	if (_stackTokens.size() > ruleSize)
	{
		TkType stackPrevType = _stackTokens[_stackTokens.size() - ruleSize - 1].getToken()._type;
		prevGood = _prevToBottomTrans.hasArrow(
			stackPrevType, ruleResultType);
	}
	if (_nextInputIndex < _inputTokens->size())
	{
		nextGood = _topToNextTrans.hasArrow(ruleResultType, getNextTkType());
	}
	return prevGood && nextGood;
}

TkType Parser::getNextTkType() const
{
	if (_nextInputIndex >= _inputTokens->size())
	{
		return TKTYPE_NONE;
	}
	else
	{
		return _inputTokens->operator[](_nextInputIndex)._type;
	}
}

ParseTree Parser::parse(vector<Token> &tokens)
{
	if (!_rulesetInitialized)
	{
		initialize();
		_rulesetInitialized = true;
	}

	_inputTokens = &tokens;
	_stackTokens = ParseStack(tokens.size());
	_nextInputIndex = 0;

	while (true)
	{
		if (_nextInputIndex >= _inputTokens->size())
		{
			auto ruleMatch = getRuleMatch();
			if (ruleMatch >= 0)
			{
				_stackTokens.reduce(_rules[ruleMatch]);
				if (_stackTokens.size() == 1)
				{
					if (_logSuccess)
					{
						cout << "Parser SUCCESS!!!" << endl;
					}
					return _stackTokens[0];
				}
			}
			cout << "Parser FAILED! Here's the stack:" << endl;
			for (size_t i = 0; i < _stackTokens.size(); i++)
			{
				_stackTokens[i].print(_tkTypeReprs, 2, false);
			}
			CRASH("Parser failed!");
		}

		auto ruleMatch = getRuleMatch();
		if (ruleMatch >= 0)
		{
			_stackTokens.reduce(_rules[ruleMatch]);
		}
		else
		{
			_stackTokens.pushToken(_inputTokens->operator[](_nextInputIndex++));
			if (_nextInputIndex < _inputTokens->size() &&
				isNextTokenUnexpected())
			{
				auto &badtok = _inputTokens->operator[](_nextInputIndex);
				cout << "Unexpected token of type ";
				cout << getTkTypeRepr(badtok._type);
				cout << endl
					 << "  Line number " << badtok._lineNumber;
				cout << endl
					 << "  Column number " << badtok._columnNumber;
				cout << endl
					 << "  Parsed stuff so far:"
					 << endl;
				for (size_t i = 0; i < _stackTokens.size(); i++)
				{
					_stackTokens[i].print(_tkTypeReprs, 2);
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