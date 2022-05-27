#include "JurassicEffect.hpp"
#include "Common/All.hpp"
#include "ITrex.hpp"
#include "Expr/Rex.hpp"
#include "Expr/RexProcessContext.hpp"
using namespace std;


// JurassicEffect::JurassicEffect()
// {
// }

JurassicEffect::JurassicEffect(RexProcessContext &prog)
{
	prog._resultsMap.set(Rex::getInputVar(SynthInputVarType::EFFECTINPUTVAR, 0).getResultPlaceholder(), &_leftInput);
	prog._resultsMap.set(Rex::getInputVar(SynthInputVarType::EFFECTINPUTVAR, 1).getResultPlaceholder(), &_rightInput);
	prog._resultsMap.flagAsChanging(&_leftInput);
	prog._resultsMap.flagAsChanging(&_rightInput);

	construct(prog);

	Validate();
	initialize();
}

JurassicEffect::~JurassicEffect()
{
}

void JurassicEffect::Validate()
{
	set<crealp> validArgs;
	validArgs.insert(&_leftInput);
	validArgs.insert(&_rightInput);
	for(auto trex : _allTrexes)
	{
		validArgs.insert(trex->getResult());
	}
	int numfdsaijf = 0;
	for(auto trex : _allTrexes)
	{
		for(auto arg : trex->getArgs())
		{
			if (validArgs.find(*arg) == validArgs.end() && !g_valuesBank.isConst(*arg))
			{
				for(auto x : Rex::g_inputVars)
				{
					for(auto y : x)
					{
						if (y.getResultPlaceholder() == *arg)
						{
							CRASH("JurassicEffect not valid! arg bad");
						}
					}
				}
				for(auto x : Rex::g_customFunParams)
				{
					if (x.getResultPlaceholder() == *arg)
					{
						CRASH("JurassicEffect not valid! arg bad");
					}
				}
				for(auto x : Rex::g_namedVars)
				{
					if (x.second.getResultPlaceholder() == *arg)
					{
						CRASH("JurassicEffect not valid! arg bad");
					}
				}
				CRASH("JurassicEffect not valid! arg bad");
			}
		}
		numfdsaijf ++;
	}
	if (validArgs.find(_forLeft) == validArgs.end())
	{
		CRASH("JurassicEffect not valid! _forLeft bad");
	}
	if (validArgs.find(_forRight) == validArgs.end())
	{
		CRASH("JurassicEffect not valid! _forRight bad");
	}
}

void JurassicEffect::initialize()
{
	FASTFOR_SIZE(_preTrexesArray,p)
	{
		(*p)->eval();
	}
}

void JurassicEffect::genSample()
{
	FASTFOR_SIZE(_trexesArray,p)
	{
		(*p)->eval();
	}
	_left = *_forLeft;
	_right = *_forRight;
}
