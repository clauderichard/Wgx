#include "JurassicEffect.hpp"
#include "Common/All.hpp"
#include "ITrex.hpp"
#include "ValuesSets.hpp"
using namespace std;


JurassicEffect::JurassicEffect()
{
}

JurassicEffect::~JurassicEffect()
{
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
