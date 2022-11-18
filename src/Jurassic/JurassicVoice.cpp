#include "JurassicVoice.hpp"
#include "Common/All.hpp"
#include "Expr/Rex.hpp"
#include "Expr/RexProcessContext.hpp"
#include "ITrex.hpp"
using namespace std;

JurassicVoice::JurassicVoice(size_t numParams)
	: _params(numParams)
{
}

JurassicVoice::JurassicVoice(size_t numParams, RexProcessContext &prog)
	: _params(numParams)
{
	RealpMap &m = prog._resultsMap;
	for (size_t i = 0; i < numParams; i++)
	{
		m.set(Rex::getInputVar(SynthInputVarType::VOICEINPUTVAR, i).getResultPlaceholder(), getParam(i));
	}
	construct(prog);
	Validate();
}

JurassicVoice::~JurassicVoice()
{
}

void JurassicVoice::Validate()
{
	set<crealp> validArgs;
	for (size_t i = 0; i < _params.size(); i++)
	{
		validArgs.insert(&_params[i]);
	}
	for(auto trex : _allTrexes)
	{
		validArgs.insert(trex->getResult());
	}
	int numfdsaijf = 0;
	for(auto trex : _allTrexes)
	{
		for(auto arg : trex->getArgs())
		{
			if (validArgs.find(*arg) == validArgs.end() && !g_valuesBank.isConst(*arg) && !g_valuesBank.isChangingVar(*arg))
			{
				CRASH("JurassicVoice not valid! arg bad");
			}
		}
		numfdsaijf ++;
	}
	if (validArgs.find(_forLeft) == validArgs.end())
	{
		CRASH("JurassicVoice not valid! _forLeft bad");
	}
	if (validArgs.find(_forRight) == validArgs.end())
	{
		CRASH("JurassicVoice not valid! _forRight bad");
	}
}

JurassicVoice *JurassicVoice::copy()
{
	JurassicVoice *ret = new JurassicVoice(_params.size());
	RealpMap m;
	for (size_t i = 0; i < _params.size(); i++)
	{
		m.set(&_params[i], &(ret->_params[i]));
	}
	FASTFOR_SIZE(_preTrexesArray, p)
	{
		shared_ptr<ITrex> newTrex((*p)->copy());
		m.set((*p)->getResult(), newTrex->getResult());
		ret->addPreTrex(newTrex);
	}
	FASTFOR_SIZE(_trexesArray, p)
	{
		shared_ptr<ITrex> newTrex((*p)->copy());
		m.set((*p)->getResult(), newTrex->getResult());
		ret->addRuntimeTrex(newTrex);
		if (_forLeft == (*p)->getResult())
		{
			ret->_forLeft = (crealp)newTrex->getResult();
		}
		if (_forRight == (*p)->getResult())
		{
			ret->_forRight = (crealp)newTrex->getResult();
		}
	}
	for(size_t i=0; i<ret->_allTrexes.size(); i++)
	{
		auto trex = ret->_allTrexes[i];
		trex->updateArgs(m);
		trex->initialize();
	}
	ret->Validate();
	return ret;
}

const double *JurassicVoice::getParam(size_t index)
{
	return &_params[index];
}

void JurassicVoice::resetParams(size_t numSamples, initializer_list<double> params)
{
	if (numSamples == 0)
	{
		_numSamplesIsFinite = false;
	}
	else
	{
		_numSamplesIsFinite = true;
		_numSamplesRemaining = numSamples;
	}
	size_t i = 0;
	for (auto p : params)
	{
		_params[i] = p;
		i++;
	}
	FASTFOR_SIZE(_preTrexesArray, p)
	{
		(*p)->eval();
	}

	reset();
}

void JurassicVoice::genSample()
{
	if (_numSamplesIsFinite && _numSamplesRemaining-- == 0)
	{
		throw false; // done playing!
	}
	FASTFOR_SIZE(_trexesArray, p)
	{
		(*p)->eval();
	}
	_left = *_forLeft;
	_right = *_forRight;
}

void JurassicVoice::reset()
{
	FASTFOR_SIZE(_trexesArray, p)
	{
		(*p)->reset();
	}
}
