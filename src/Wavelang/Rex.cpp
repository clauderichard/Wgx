#include "Common/All.hpp"
#include "Rex.hpp"
#include "ITrex.hpp"
//#include "TrexSpecies.hpp"
#include "ValuesSets.hpp"
#include "JurassicWorld.hpp"

shared_ptr<JurassicPark> Rex::g_currentJurassicPark;

void Rex::useNewPark()
{
	Rex::g_currentJurassicPark.reset(new JurassicPark());
}

crealp Rex::mkRex(ITrex *trex)
{
	Rex::g_currentJurassicPark->addTrex(trex);
	return trex->getResult();
}

Rex::Rex()
	: _val(NULL),
	  _myPark(NULL) {}

Rex::Rex(crealp p, shared_ptr<JurassicPark> park)
	: _val(p),
	  _myPark(park)
{
}

Rex::Rex(double c)
	: _val(g_constDoubles.getOrAdd(c)),
	  _myPark(NULL) {}

Rex Rex::copy()
{
	CRASH("why.")
	if (!_myPark)
	{
		return Rex(_val, NULL);
	}
	RealpMap m;
	return Rex(m[_val], _myPark->copy(m));
}

vector<crealp> Rex::getVariableDependencies()
{
	if (g_namedVarsDoubles.contains(_val))
	{
		return vector<crealp>{_val};
	}
	return _myPark->getVariableDependencies();
}

vector<crealp> Rex::getInputDependencies()
{
	if (g_voiceInputVars.contains(_val) || g_effectInputVars.contains(_val))
	{
		return vector<crealp>{_val};
	}
	return _myPark->getInputDependencies();
}

void Rex::print() const
{
	if (g_constDoubles.contains(_val))
	{
		cout << *_val << endl;
	}
	else if (g_namedVarsDoubles.contains(_val))
	{
		cout << _val << endl;
	}
	else if (g_voiceInputVars.contains(_val))
	{
		cout << _val << endl;
	}
	else if (g_effectInputVars.contains(_val))
	{
		cout << _val << endl;
	}
	else if (!_myPark)
	{
		cout << "?" << endl;
	}
	else
	{
		cout << _val << " ";
		_myPark->print();
	}
}