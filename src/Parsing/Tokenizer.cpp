#include "Tokenizer.hpp"
#include "Common/All.hpp"
////////////////////////////////////////////////

#define ASCII_CHAR_CAP 256

TkAutomaton::TkAutomaton()
{
	addNewState();
}

TkAutomaton::~TkAutomaton() {}

TkState TkAutomaton::addChar(bool (*charFunc)(char),
						   TkType type,
						   TkState fromState)
{
	TkState toState = addNewState();
	for (int i = 0; i < ASCII_CHAR_CAP; i++)
	{
		char c = (char)i;
		if (!charFunc(c))
		{
			continue;
		}
		setTransit(fromState, c, toState);
	}
	_stateToType[toState] = type;
	return toState;
}

TkState TkAutomaton::addWord(const string &word,
						   TkType type,
						   TkState fromState)
{
	TkState state = fromState;
	for (int i = 0; i < word.size(); i++)
	{
		char letter = word[i];
		state = moveOrCreateState(state, letter);
	}
	_stateToType[state] = type;
	return state;
}

TkState TkAutomaton::addStar(bool (*charFunc)(char),
						   TkType type,
						   TkState fromState)
{
	TkState starState = addNewState();
	for (int i = 0; i < ASCII_CHAR_CAP; i++)
	{
		char c = (char)i;
		if (!charFunc(c))
		{
			continue;
		}
		setTransit(fromState, c, starState);
		setTransit(starState, c, starState);
	}
	_stateToType[starState] = type;
	return starState;
}

void TkAutomaton::reset()
{
	_currentState = 0;
	_prevState = 0;
	_isDead = false;
	_isDeadAndDeprecated = false;
}

void TkAutomaton::step(char c)
{
	if (_isDead)
	{
		_isDeadAndDeprecated = true;
		return;
	}
	TkState nextState = getTransit(_currentState, c);
	_isDead = nextState == 0;
	_prevState = _currentState;
	_currentState = nextState;
}

TkType TkAutomaton::getCurrentType() const
{
	return _stateToType[_prevState];
}

////////////////////////////////////////////////

TkState TkAutomaton::moveOrCreateState(TkState state, char c)
{
	TkState ret = getTransit(state, c);
	if (ret == 0)
	{
		// create
		ret = addNewState();
		setTransit(state, c, ret);
	}
	return ret;
}

TkState TkAutomaton::addNewState()
{
	_transitions.emplace_back(ASCII_CHAR_CAP, 0);
	_stateToType.emplace_back(0);
	return (TkState)(_transitions.size() - 1);
}

TkState TkAutomaton::getTransit(TkState fromState, char c)
{
	return _transitions[fromState][c];
}

void TkAutomaton::setTransit(TkState fromState, char c, TkState toState)
{
	_transitions[fromState][c] = toState;
}

////////////////////////////////////////////////

Tokenizer::Tokenizer()
{
	_automata.emplace_back(new TkAutomaton());
	_automata.emplace_back(new TkAutomaton());
	addNewState();
}

Tokenizer::~Tokenizer() {}

TkState Tokenizer::addPostStarWord(const string &word,
						   TkType type,
						   TkState fromState)
{
	return _automata[1]->addWord(word, type, fromState);
}

TkState Tokenizer::addChar(bool (*charFunc)(char),
						   TkType type,
						   TkState fromState)
{
	return _automata[1]->addChar(charFunc, type, fromState);
}

TkState Tokenizer::addWord(const string &word,
						   TkType type,
						   TkState fromState)
{
	return _automata[0]->addWord(word, type, fromState);
}

TkState Tokenizer::addStar(bool (*charFunc)(char),
						   TkType type,
						   TkState fromState)
{
	return _automata[1]->addStar(charFunc, type, fromState);
	clearTempStateBools();
	fillTempStateBoolsWithPath(charFunc, fromState);
	TkState starState = addNewState();
	for (TkState state = 0; state < _transitions.size(); state++)
	{
		if (!_tempStateBools[state])
		{
			continue;
		}
		if (_stateToType[state] == 0)
		{
			_stateToType[state] = type;
		}
		for (int i = 0; i < ASCII_CHAR_CAP; i++)
		{
			char c = (char)i;
			if (!charFunc(c) || transit(state, c) != 0)
			{
				continue;
			}
			setTransit(state, c, starState);
		}
	}
	for (int i = 0; i < ASCII_CHAR_CAP; i++)
	{
		char c = (char)i;
		if (!charFunc(c))
		{
			continue;
		}
		setTransit(starState, c, starState);
	}
	_stateToType[starState] = type;
	return starState;
}

void Tokenizer::tokenizeLine(vector<Token> &ret, const string &inputString)
{
	// TkState curState = 0;
	size_t inputCharsCurrent = 0;
	size_t inputCharsSize = inputString.size();
	char nextInputChar = inputString[inputCharsCurrent];
	Token nextToken;
	size_t startIndex = 0;
	size_t length = 0;
	nextToken._lineNumber = 0;
	nextToken._columnNumber = 0;
	size_t lineStart = 0;
	
	// revive automata
	for(size_t a=0; a<_automata.size(); a++)
	{
		_automata[a]->reset();
	}

	for(size_t i=0; i<inputCharsSize;i++)
	{
		char inp = inputString[i];

		bool someAlive = false;
		for(size_t a=0; a<_automata.size(); a++)
		{
			_automata[a]->step(inp);
			someAlive = someAlive || !_automata[a]->_isDead;
		}

		// TkType nextState = (TkType)transit(curState, inp);

		if (!someAlive)
		{
			if (length > 0)
			{
				nextToken._string.reset(new string(inputString.substr(startIndex, length)));
				pushToken(ret, nextToken, lineStart, startIndex);
				startIndex += length;
				length = 0;
				// revive automata
				for(size_t a=0; a<_automata.size(); a++)
				{
					_automata[a]->reset();
				}
				// will try again
				i--;
			}
			else
			{
				startIndex = i+1;
				// revive automata
				for(size_t a=0; a<_automata.size(); a++)
				{
					_automata[a]->reset();
				}
			}
		}
		else
		{
			// accumulate
			// curState = nextState;
			length++;
		}
		if (nextInputChar == '\n')
		{
			nextToken._lineNumber++;
			lineStart = inputCharsCurrent;
		}
	}
	if (length > 0)
	{
		for(size_t a=0; a<_automata.size(); a++)
		{
			_automata[a]->step('\t');
		}
		nextToken._string.reset(new string(inputString.substr(startIndex, length)));
		pushToken(ret, nextToken, lineStart, startIndex);
		startIndex += length;
		length = 0;
	}
}

void Tokenizer::pushToken(vector<Token> &tokens, Token &token, size_t lineStart, size_t startIndex) const
{
	// pick first valid automaton. (they're all dead)
	for(size_t a=0; a<_automata.size(); a++)
	{
		if (!_automata[a]->_isDeadAndDeprecated && _automata[a]->getCurrentType() != 0)
		{
			token._type = _automata[a]->getCurrentType();
			break;
		}
	}
	token._columnNumber = startIndex - lineStart;
	tokens.push_back(token);
}

////////////////////////////////////////////////

void Tokenizer::initialize()
{
	//NOTIMPL("init toknzer")
}

void Tokenizer::clearTempStateBools()
{
	for (size_t i = 0; i < _transitions.size(); i++)
	{
		_tempStateBools[i] = false;
	}
}

void Tokenizer::fillTempStateBoolsWithPath(bool (*charFunc)(char),
										   TkState fromState)
{
	if (_tempStateBools[fromState])
	{
		return;
	}
	_tempStateBools[fromState] = true;
	for (int i = 0; i < ASCII_CHAR_CAP; i++)
	{
		char c = (char)i;
		if (!charFunc(c))
		{
			continue;
		}
		TkState next = transit(fromState, c);
		fillTempStateBoolsWithPath(charFunc, next);
	}
}

TkState Tokenizer::moveOrCreateState(TkState state, char c)
{
	TkState ret = transit(state, c);
	if (ret == 0)
	{
		// create
		ret = addNewState();
		setTransit(state, c, ret);
	}
	return ret;
}

TkState Tokenizer::addNewState()
{
	_transitions.emplace_back(ASCII_CHAR_CAP, 0);
	_stateToType.emplace_back(0);
	_tempStateBools.emplace_back(false);
	return (TkState)(_transitions.size() - 1);
}

TkState Tokenizer::transit(TkState fromState, char c)
{
	return _transitions[fromState][c];
}
void Tokenizer::setTransit(TkState fromState, char c, TkState toState)
{
	_transitions[fromState][c] = toState;
}

////////////////////////////////////////////////
