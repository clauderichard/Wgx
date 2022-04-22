#ifndef __JURASSICEFFECT_HPP__
#define __JURASSICEFFECT_HPP__

#include <vector>
#include <map>
#include "Realp.hpp"
#include "Wave/IEffect.hpp"
#include "IJurassicSynthElement.hpp"
#include "Util/FastArray.hpp"

using namespace std;

class ITrex;

class JurassicEffect : public IEffect, public IJurassicSynthElement
{
  public:
	JurassicEffect();
	virtual ~JurassicEffect();

	void initialize();

	// This one should be super efficient,
	// because you will probably call this 44,100 times per second.
	void genSample();

};

#endif