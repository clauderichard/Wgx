#include "Common/All.hpp"
#include "JurassicWorld.hpp"
#include "JurassicVoice.hpp"
#include "JurassicEffect.hpp"
#include "IJurassicSynthElement.hpp"
#include "ITrex.hpp"
#include "ValuesSets.hpp"
using namespace std;

////////////////////////////////////////////////

JurassicPark::JurassicPark()
	: _trexesArray(CAPACITY_JURASSICPARK_TREX),
	  _result(g_constDoubles.getOrAdd(0))
{
}

shared_ptr<JurassicPark> JurassicPark::copy(RealpMap &m)
{
	shared_ptr<JurassicPark> ret(new JurassicPark());
	FASTFOR_SIZE(_trexesArray, p)
	{
		auto newTrex = (*p)->copyAndAddToMap(m);
		ret->addTrex(newTrex);
	}
	return ret;
}

void JurassicPark::addTrex(ITrex *trex)
{
	shared_ptr<ITrex> x(trex);
	_trexesArray.pushBack(x);
}

vector<crealp> JurassicPark::getVariableDependencies()
{
	vector<crealp> ret;
	FASTFOR_SIZE(_trexesArray, p)
	{
		auto args = (*p)->getArgs();
		for (size_t i = 0; i < args.size(); i++)
		{
			if (g_namedVarsDoubles.contains(*args[i]))
			{
				ret.push_back(*args[i]);
			}
		}
	}
	return ret;
}

vector<crealp> JurassicPark::getInputDependencies()
{
	vector<crealp> ret;
	FASTFOR_SIZE(_trexesArray, p)
	{
		auto args = (*p)->getArgs();
		for (size_t i = 0; i < args.size(); i++)
		{
			if (g_effectInputVars.contains(*args[i]) ||
				g_voiceInputVars.contains(*args[i]))
			{
				ret.push_back(*args[i]);
			}
		}
	}
	return ret;
}

void JurassicPark::print() const
{
	CrealpNamer n;
	cout << "  JurassicPark trexes:" << endl;
	FASTFOR_SIZE(_trexesArray, p)
	{
		crealp x = (*p)->getResult();
		n.addName(x);
		cout << x << " " << n[x] << " = ";
		(*p)->print(n);
		cout << endl;
	}
}

////////////////////////////////////////////////

JurassicWorld::JurassicWorld() {}

JurassicWorld::~JurassicWorld()
{
}

shared_ptr<JurassicWorld> JurassicWorld::copy()
{
	shared_ptr<JurassicWorld> ret(new JurassicWorld());
	RealpMap m;
	for (auto &x : _nameToRexMap)
	{
		ret->_nameToRexMap[x.first] = x.second;
	}
	return ret;
}

void JurassicWorld::clear()
{
	_nameToRexMap.clear();
}
void JurassicWorld::addRex(const string &name, Rex &rex)
{
	_nameToRexMap[name] = rex;
}
void JurassicWorld::addAllRexesFrom(shared_ptr<JurassicWorld> other)
{
	for (auto x : other->_nameToRexMap)
	{
		_nameToRexMap[x.first] = x.second;
		//auto newRex = x.second.copy();
		//addRex(x.first, newRex);
	}
}

bool leafIsValidVoiceInput(crealp x)
{
	return !g_effectInputVars.contains(x);
}
bool leafIsValidEffectInput(crealp x)
{
	return !g_voiceInputVars.contains(x);
}

bool JurassicWorld::isValidVoice()
{
	//set<string> checked;
	return andForAllDependencies(leafIsValidVoiceInput);
}

bool JurassicWorld::isValidEffect()
{
	//set<string> checked;
	return andForAllDependencies(leafIsValidEffectInput);
}

vector<string> JurassicWorld::getRexDependenciesOrder()
{
	bool hasMainL = hasNamedWorld(KEYWORD_MAINLEFT);
	bool hasMainR = hasNamedWorld(KEYWORD_MAINRIGHT);
	bool hasMain = hasNamedWorld(KEYWORD_MAIN);
	bool bothSidesCovered = hasMain || (hasMainL && hasMainR);

	if (!bothSidesCovered)
	{
		throw out_of_range("outputs not both there");
	}

	bool mainUsed = hasMain && (!hasMainL || !hasMainR);

	vector<string> ret;
	set<string> done;
	if (mainUsed)
	{
		addRexDependenciesOrder(ret, done, KEYWORD_MAIN);
	}
	if (hasMainL)
	{
		addRexDependenciesOrder(ret, done, KEYWORD_MAINLEFT);
	}
	if (hasMainR)
	{
		addRexDependenciesOrder(ret, done, KEYWORD_MAINRIGHT);
	}
	return ret;
}
void JurassicWorld::addRexDependenciesOrder(
	vector<string> &ret,
	set<string> &done,
	const string &rexNameToAdd)
{
	if (done.find(rexNameToAdd) != done.end())
	{
		return;
	}
	done.insert(rexNameToAdd);
	Rex &rexToAdd = _nameToRexMap.at(rexNameToAdd);

	// add dependencies
	auto deps = rexToAdd.getVariableDependencies();
	for (auto dep : deps)
	{
		string depName = g_namedVarsDoubles.getNameOf(dep);
		addRexDependenciesOrder(
			ret, done, depName);
	}
	ret.push_back(rexNameToAdd);
}

// For every leaf dependency (e.g. $inputL or $pitch)
bool JurassicWorld::andForAllDependencies(bool (*f)(crealp))
{
	try
	{
		auto depss = getRexDependenciesOrder();

		for (auto dep : depss)
		{
			// check $ dependencies (directly under this)
			auto inputs = _nameToRexMap.at(dep).getInputDependencies();
			for (auto inp : inputs)
			{
				if (!f(inp))
				{
					return false;
				}
			}
		}
	}
	catch (out_of_range)
	{
		return false;
	}

	return true;
}

bool JurassicWorld::hasNamedWorld(const string &name)
{
	return _nameToRexMap.find(name) != _nameToRexMap.end();
}

shared_ptr<IVoice> JurassicWorld::toVoice()
{
	//cout<<"calling JurassicWorld::toVoice on this world:"<<endl;
	//print();
	//cout<<endl;
	size_t numParams = CAPACITY_JURASSICVOICE_PARAMS;
	JurassicVoice *ret = new JurassicVoice(numParams);

	RealpMap m;
	set<crealp> changingValues;
	for (size_t i = 0; i < numParams; i++)
	{
		m.set(g_voiceInputVars.get(i), ret->getParam(i));
	}

	addAllToSynthElement(ret, m, changingValues);

	//cout<< "RealpMap m:"<<endl;
	//m.print();

	return shared_ptr<IVoice>(ret);
}

shared_ptr<IEffect> JurassicWorld::toEffect()
{
	JurassicEffect *ret = new JurassicEffect();

	RealpMap m;
	set<crealp> changingValues;
	m.set(g_effectInputVars.get(0), ret->getLeftInputPtr());
	m.set(g_effectInputVars.get(1), ret->getRightInputPtr());
	changingValues.insert(ret->getLeftInputPtr());
	changingValues.insert(ret->getRightInputPtr());

	addAllToSynthElement(ret, m, changingValues);

	return shared_ptr<IEffect>(ret);
}

void JurassicWorld::addAllToSynthElement(IJurassicSynthElement *ret, RealpMap &m, set<crealp> &changingValues)
{
	ret->_forLeft = NULL;
	ret->_forRight = NULL;
	auto deps = getRexDependenciesOrder();
	for (auto dep : deps)
	{
		Rex &rexToAdd = _nameToRexMap.at(dep);
		ITrex *latestTrex = NULL;
		if (rexToAdd._myPark)
		{
			FASTFOR_SIZE(rexToAdd._myPark->_trexesArray, p)
			{
				latestTrex = (*p)->copyAndAddToMap(m);

				if (resultIsChanging(latestTrex, changingValues))
				{
					changingValues.insert(latestTrex->getResult());
					//cout<<"changingValues now:"<<endl;
					//for(auto x : changingValues)
					//				{
					//					cout<<"  "<<x<<endl;
					//				}
					//				cout<<"realpMap m now:"<<endl;
					//				m.print();
					ret->addTrex(latestTrex);
				}
				else
				{
					ret->addPreTrex(latestTrex);
				}
			}
		}
		crealp result = m[rexToAdd._val];
		m.set(g_namedVarsDoubles.getOrAdd(dep), result);
		if (dep == KEYWORD_MAINLEFT)
		{
			ret->_forLeft = result;
		}
		else if (dep == KEYWORD_MAINRIGHT)
		{
			ret->_forRight = result;
		}
		else if (dep == KEYWORD_MAIN)
		{
			if (ret->_forLeft == NULL)
				ret->_forLeft = result;
			if (ret->_forRight == NULL)
				ret->_forRight = result;
		}
	}
	ret->updateAllArgs(m);
}

bool JurassicWorld::resultIsChanging(
	ITrex *resultTrex,
	set<crealp> &changingValues)
{
	//cout << "is result changing?  " << resultTrex->getResult() << endl;

	//logVal(g_effectInputVars.size());
	if (resultTrex->introducesChange()) // e.g. if it's a PhaseTrex
	{
		//cout << "introducesFhange " << resultTrex->getResult() << endl;
		return true;
	}
	// If any args are changing, then result is changing.
	auto latestArgs = resultTrex->getArgs();
	for (auto arg : latestArgs)
	{
		//cout << "  checking arg " << arg << endl;
		//if (g_voiceInputVars.contains(*arg))
		//		{
		//			CRASH("Var found in a JurassicVoice!?")
		//		}
		if (g_effectInputVars.contains(*arg))
		{
			//cout << "arg is effectInputVar so this changing" << endl;
			return true;
		}
		if (changingValues.find(*arg) != changingValues.end())
		{
			//cout << "arg is in changingValues so this changing" << endl;
			return true;
		}
	}
	if (changingValues.find(resultTrex->getResult()) != changingValues.end())
	{
		//cout << "esultTrex is in changingValues so this changing" << endl;
		return true;
	}
	//cout << "not changing" << resultTrex->getResult() << endl;

	return false;
}

void JurassicWorld::print() const
{
	cout << endl
		 << "    JurassicWorld" << endl;
	for (auto x : _nameToRexMap)
	{
		cout << "   " << g_namedVarsDoubles.getOrAdd(x.first) << " " << x.first << ":" << endl;
		x.second.print();
	}
}

////////////////////////////////////////////////
