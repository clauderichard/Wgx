#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include "Tokenizer.hpp"
#include "Traverser.hpp"
#include "Parser.hpp"
#include "Token.hpp"
#include "ParseTree.hpp"
#include "Value.hpp"
#include <cstddef>
#include <string>
using namespace std;

////////////////////////////////////////////////

class Language
{
  public:
	Language(size_t tkStatesCapacity);

	////////////////////////////////////////
	// Build Tokenizer

	void setTkTypeRepr(TkType tkType, const string &repr);

	TkState addWord(const string &word,
					TkType resultType);

	TkState addPostStarWord(const string &word,
							TkType resultType,
							TkState fromState);

	template <typename R>
	TkState addWordValue(const string &word,
						 TkType resultType,
						 R value)
	{
		TkType valTkType = _unusedTkType--;
		PrRuleId ruleId = _unusedRuleId++;
		_parser.setTkTypeRepr(valTkType, word);
		_parser.addRule(resultType, {valTkType}, ruleId, {0});
		_traverser.addLeafInstr(valTkType,
								new TravLeafValue(Value::make<R>(value)));
		_traverser.addNodeIdentityFunc(ruleId);
		return _tokenizer.addWord(word, valTkType);
	}

	template <typename R>
	TkState addWord(const string &word,
					TkType resultType,
					R (*f)(const string &))
	{
		_parser.setTkTypeRepr(resultType, word);

		_traverser.addLeafFunc(resultType, f);
		return _tokenizer.addWord(word, resultType);
	}

	////////////////////////////////////////
	// Fancier Tokenizer rules

	TkState addCharStar(bool (*charFunc)(char),
						TkType resultType,
						TkState fromState = 0);

	template <typename R>
	TkState addChar(bool (*charFunc)(char),
						TkType resultType,
						R (*f)(const string &),
						TkState fromState = 0)
	{
		_traverser.addLeafFunc(resultType, f);
		return _tokenizer.addChar(charFunc, resultType, fromState);
	}

	template <typename R>
	TkState addCharStar(bool (*charFunc)(char),
						TkType resultType,
						R (*f)(const string &),
						TkState fromState = 0)
	{
		_traverser.addLeafFunc(resultType, f);
		return _tokenizer.addStar(charFunc, resultType, fromState);
	}

	////////////////////////////////////////
	// Build Parser

	PrRuleId addUnaryParseRule(
		TkType resultType,
		initializer_list<TkType> stackTypes,
		size_t valuedChildIndex)
	{
		PrRuleId ruleId = _unusedRuleId++;
		_parser.addRule(resultType, stackTypes, ruleId, {valuedChildIndex});
		_traverser.addNodeIdentityFunc(ruleId);
		return ruleId;
	}

	PrRuleId addUnaryParseRule(
		TkType resultType,
		TkType stackType)
	{
		return addUnaryParseRule(resultType, {stackType}, 0);
	}

	template <typename R>
	PrRuleId addParseRule(
		TkType resultType,
		initializer_list<TkType> stackTypes,
		initializer_list<size_t> valuedChildIndexes,
		R (*f)())
	{
		PrRuleId ruleId = _unusedRuleId++;
		_parser.addRule(resultType, stackTypes, ruleId, valuedChildIndexes);
		_traverser.addNodeFunc(ruleId, f);
		return ruleId;
	}

	template <typename R, typename A>
	PrRuleId addParseRule(
		TkType resultType,
		initializer_list<TkType> stackTypes,
		initializer_list<size_t> valuedChildIndexes,
		R (*f)(A &))
	{
		PrRuleId ruleId = _unusedRuleId++;
		_parser.addRule(resultType, stackTypes, ruleId, valuedChildIndexes);
		_traverser.addNodeFunc(ruleId, f);
		return ruleId;
	}

	template <typename R, typename A, typename B>
	PrRuleId addParseRule(
		TkType resultType,
		initializer_list<TkType> stackTypes,
		initializer_list<size_t> valuedChildIndexes,
		R (*f)(A &, B &))
	{
		PrRuleId ruleId = _unusedRuleId++;
		_parser.addRule(resultType, stackTypes, ruleId, valuedChildIndexes);
		_traverser.addNodeFunc(ruleId, f);
		return ruleId;
	}

	template <typename R, typename A, typename B, typename C>
	PrRuleId addParseRule(
		TkType resultType,
		initializer_list<TkType> stackTypes,
		initializer_list<size_t> valuedChildIndexes,
		R (*f)(A &, B &, C &))
	{
		PrRuleId ruleId = _unusedRuleId++;
		_parser.addRule(resultType, stackTypes, ruleId, valuedChildIndexes);
		_traverser.addNodeFunc(ruleId, f);
		return ruleId;
	}

	////////////////////////////////////////
	// Fancier Parser rules

	template <typename R, typename A>
	PrRuleId addListRule(TkType listType,
						 TkType leftDelimiter,
						 TkType elementType,
						 TkType rightDelimiter,
						 R (*vecFun)(vector<A> &))
	{
		TkType starType = _unusedTkType--;
		PrRuleId initRuleId = _unusedRuleId++;
		PrRuleId appendRuleId = _unusedRuleId++;
		PrRuleId endRuleId = _unusedRuleId++;
		_parser.addRule(starType, {leftDelimiter}, initRuleId, {});
		_parser.addRule(starType, {starType, elementType}, appendRuleId, {0, 1});
		_parser.addRule(listType, {starType, rightDelimiter}, endRuleId, {0});
		_traverser.addNodeListFunc(initRuleId, endRuleId, vecFun);
		return endRuleId;
	}

	// S A (B A)* E
	template <typename R, typename A, typename B>
	PrRuleId addListRule2(TkType listType,
						  TkType startType,
						  TkType elementType,
						  TkType betweenType,
						  TkType endType,
						  R (*vecFun)(vector<A> &, vector<B> &))
	{
		TkType buildType = _unusedTkType--;
		PrRuleId initRuleId = _unusedRuleId++;
		PrRuleId appendRuleId = _unusedRuleId++;
		PrRuleId listRuleId = _unusedRuleId++;
		_parser.addRule(buildType, {startType, elementType}, initRuleId, {1});
		_parser.addRule(buildType, {buildType, betweenType, elementType}, appendRuleId, {});
		_parser.addRule(listType, {buildType, endType}, listRuleId, {0});
		_traverser.addNodeListFunc2(initRuleId, listRuleId, vecFun);
		return appendRuleId;
	}

	////////////////////////////////////////
	// Parser settings

	void setLogSuccess(bool logSuccess);

	////////////////////////////////////////
	// Traverser settings

	void addNodePreFunc(PrRuleId ruleId,
						void (*fun)())
	{
		_traverser.addNodePreFunc(ruleId, fun);
	}

	////////////////////////////////////////
	// Process

	Value interpretFile(const string &code);
	Value interpret(const string &code);

	////////////////////////////////////////

  private:
	Tokenizer _tokenizer;
	Parser _parser;
	Traverser _traverser;

	PrRuleId _unusedRuleId;
	TkType _unusedTkType;
};

////////////////////////////////////////////////

#endif