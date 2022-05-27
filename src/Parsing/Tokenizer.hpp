#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__

#include <vector>
#include <string>
#include <memory>
#include "Token.hpp"
#include "TypeDefs.hpp"
using namespace std;

////////////////////////////////////////////////

class TkAutomaton
{
  public:
	TkAutomaton();
	~TkAutomaton();

	// return toState
	TkState addChar(bool (*charFunc)(char),
					TkType type,
					TkState fromState = 0);
	TkState addWord(const string &word,
					TkType type,
					TkState fromState = 0);
	TkState addStar(bool (*charFunc)(char),
					TkType type,
					TkState fromState = 0);
	void reset();
	void step(char c);
	TkType getCurrentType() const;
  private:
	void setTransit(TkState fromState, char c, TkState toState);
	TkState moveOrCreateState(TkState state, char c);
	TkState addNewState();

	TkState getTransit(TkState fromState, char c);

	vector<vector<TkState>> _transitions;
	vector<TkType> _stateToType;
	// current
  public:
	TkState _currentState;
	TkState _prevState;
	bool _isDead;
	bool _isDeadAndDeprecated;
};

////////////////////////////////////////////////

class Tokenizer
{
  public:
	Tokenizer();
	~Tokenizer();

	// return toState
	TkState addPostStarWord(const string &word,
					TkType type,
					TkState fromState);
	TkState addChar(bool (*charFunc)(char),
					TkType type,
					TkState fromState = 0);
	TkState addWord(const string &word,
					TkType type,
					TkState fromState = 0);
	TkState addStar(bool (*charFunc)(char),
					TkType type,
					TkState fromState = 0);

	void tokenizeLine(vector<Token> &ret, const string &inputString);
	
  private:
    void initialize();
	void clearTempStateBools();
	void fillTempStateBoolsWithPath(
		bool (*charFunc)(char),
		TkState fromState);
	TkState moveOrCreateState(TkState state, char c);
	TkState addNewState();

	TkState transit(TkState fromState, char c);
	void setTransit(TkState fromState, char c, TkState toState);

	void pushToken(vector<Token> &tokens, Token &token, size_t lineStart, size_t startIndex) const;

	vector<shared_ptr<TkAutomaton>> _automata;
	vector<vector<TkState>> _transitions;
	vector<TkType> _stateToType;
	vector<bool> _tempStateBools;
};

////////////////////////////////////////////////

#endif