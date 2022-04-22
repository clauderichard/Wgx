#include "JurassicVoice.hpp"
#include "Common/All.hpp"
#include "ITrex.hpp"
#include "ValuesSets.hpp"
using namespace std;

JurassicVoice::JurassicVoice(size_t numParams)
	: _params(numParams)
{
}

JurassicVoice::~JurassicVoice()
{
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
		auto newTrex = (*p)->copyAndAddToMap(m);
		ret->addPreTrex(newTrex);
	}
	FASTFOR_SIZE(_trexesArray, p)
	{
		auto newTrex = (*p)->copyAndAddToMap(m);
		ret->addTrex(newTrex);
		if (_forLeft == (*p)->getResult())
		{
			ret->_forLeft = (crealp)newTrex->getResult();
		}
		if (_forRight == (*p)->getResult())
		{
			ret->_forRight = (crealp)newTrex->getResult();
		}
	}
	return ret;
}

const double *JurassicVoice::getParam(size_t index)
{
	return &_params[index];
}

// void JurassicVoice::addPreTrex(ITrex *trex)
// {
// 	_preTrexesArray.pushBack(trex);
// }

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

// void JurassicVoice::addTrex(ITrex *trex)
// {
// 	_trexesArray.pushBack(trex);
// }

void JurassicVoice::genSample()
{
	if (_numSamplesIsFinite && _numSamplesRemaining-- == 0)
	{
		throw false; // done playing!
	}
	//cout<<"vt ";
	FASTFOR_SIZE(_trexesArray, p)
	{
		(*p)->eval();
		//cout<<*((*p)->getResult())<<" ";
	}
	_left = *_forLeft;
	_right = *_forRight;
	//cout << "vg " << _left << " " << _right << endl;
}

void JurassicVoice::reset()
{
	FASTFOR_SIZE(_trexesArray, p)
	{
		(*p)->reset();
	}
}
