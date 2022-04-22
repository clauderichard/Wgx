#include "IJurassicSynthElement.hpp"
#include "Common/All.hpp"
#include "ITrex.hpp"
#include "ValuesSets.hpp"
#include "CrealpNamer.hpp"
using namespace std;

IJurassicSynthElement::IJurassicSynthElement()
	: _preTrexesArray(CAPACITY_JURASSICSYNTHELEMENT_PRETREX),
	  _trexesArray(CAPACITY_JURASSICSYNTHELEMENT_TREX),
	  _forLeft(g_constDoubles.getOrAdd(0)),
	  _forRight(g_constDoubles.getOrAdd(0))
{
}

IJurassicSynthElement::~IJurassicSynthElement()
{
	FASTFOR_SIZE(_preTrexesArray, p)
	{
		delete *p;
	}
	FASTFOR_SIZE(_trexesArray, p)
	{
		delete *p;
	}
}

void IJurassicSynthElement::addPreTrex(ITrex *trex)
{
	_preTrexesArray.pushBack(trex);
}

void IJurassicSynthElement::addTrex(ITrex *trex)
{
	_trexesArray.pushBack(trex);
}

void IJurassicSynthElement::updateAllArgs(RealpMap &m)
{
	FASTFOR_SIZE(_preTrexesArray, p)
	{
		(*p)->updateArgs(m);
		(*p)->initialize();
	}
	FASTFOR_SIZE(_trexesArray, p)
	{
		(*p)->updateArgs(m);
		(*p)->initialize();
	}
}

void IJurassicSynthElement::print(CrealpNamer &namer)
{
	//cout <<endl<< "  SynthElement pretrexes:" << endl;
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
