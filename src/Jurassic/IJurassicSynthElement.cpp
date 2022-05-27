#include "IJurassicSynthElement.hpp"
#include "Common/All.hpp"
#include "ITrex.hpp"
#include "ValuesSets.hpp"
#include "ValuesBank.hpp"
#include "CrealpNamer.hpp"
#include "Expr/Rex.hpp"
#include "Expr/RexProcessContext.hpp"
#include "Expr/EquationSet.hpp"
using namespace std;

IJurassicSynthElement::IJurassicSynthElement()
	: _preTrexesArray(CAPACITY_JURASSICSYNTHELEMENT_PRETREX),
	  _trexesArray(CAPACITY_JURASSICSYNTHELEMENT_TREX),
	  _forLeft(g_valuesBank.getConstant(0)),
	  _forRight(g_valuesBank.getConstant(0))
{
}

IJurassicSynthElement::~IJurassicSynthElement()
{
}

void IJurassicSynthElement::construct(RexProcessContext &prog)
{
	for(size_t i = 0; i < prog._resultTrexes.size(); i++)
	{
		auto trex = prog._resultTrexes[i];
		auto trexPointer = trex.get();
		_allTrexes.push_back(trex);
		if (resultIsChanging(trexPointer, prog._resultsMap))
		{
			prog._resultsMap.flagAsChanging(trex->getResult());
			_trexesArray.pushBack(trexPointer);
		}
		else
		{
			_preTrexesArray.pushBack(trexPointer);
		}

		trex->updateArgs(prog._resultsMap);
		trex->initialize();
	}

	if (prog._eqns.contains(KEYWORD_MAIN))
	{
		_forLeft = prog._resultsMap[prog._eqns.get(KEYWORD_MAIN).getResultPlaceholder()];
		_forRight = _forLeft;
	}
	if (prog._eqns.contains(KEYWORD_MAINLEFT))
	{
		_forLeft = prog._resultsMap[prog._eqns.get(KEYWORD_MAINLEFT).getResultPlaceholder()];
	}
	if (prog._eqns.contains(KEYWORD_MAINRIGHT))
	{
		_forRight = prog._resultsMap[prog._eqns.get(KEYWORD_MAINRIGHT).getResultPlaceholder()];
	}
}

bool IJurassicSynthElement::resultIsChanging(
	ITrex *resultTrex, RealpMap &m)
{
	if (resultTrex->introducesChange()) // e.g. if it's a PhaseTrex
	{
		return true;
	}
	// If any args are changing, then result is changing.
	auto latestArgs = resultTrex->getArgs();
	for (auto arg : latestArgs)
	{
		if (m.isChanging(*arg))
		{
			return true;
		}
	}

	return false;
}

void IJurassicSynthElement::addPreTrex(shared_ptr<ITrex> trex)
{
	_allTrexes.push_back(trex);
	auto x = trex.get();
	_preTrexesArray.pushBack(x);
}

void IJurassicSynthElement::addRuntimeTrex(shared_ptr<ITrex> trex)
{
	_allTrexes.push_back(trex);
	auto x = trex.get();
	_trexesArray.pushBack(x);
}

void IJurassicSynthElement::updateAllArgs(RealpMap &m)
{
	FASTFOR_SIZE(_preTrexesArray, p)
	{
		(*p)->updateArgs(m);
	}
	FASTFOR_SIZE(_trexesArray, p)
	{
		(*p)->updateArgs(m);
	}
}

void IJurassicSynthElement::print(CrealpNamer &namer)
{
	FASTFOR_SIZE(_preTrexesArray, p)
	{
		crealp x = (*p)->getResult();
		namer.addName(x);
		cout << x << " " << namer[x] << " = ";
		(*p)->print(namer);
		cout << endl;
	}
	cout << "trexes:" << endl;
	FASTFOR_SIZE(_trexesArray, p)
	{
		crealp x = (*p)->getResult();
		namer.addName(x);
		cout << x << " " << namer[x] << " = ";
		(*p)->print(namer);
		cout << endl;
	}
}
