#ifndef __SYMPHONY_HPP__
#define __SYMPHONY_HPP__

#include "Wave/IWaveSampler.hpp"
#include "Wave/ISynthesizer.hpp"
#include "Egg/IEgg.hpp"
#include "Common/Crash.hpp"
#include "Common/SampleRate.hpp"
#include <iostream>
using namespace std;

class Symphony : public IWaveSampler
{
  public:
	Symphony(shared_ptr<ISynthesizer>, shared_ptr<IEgg>);
	~Symphony();

	void genSample();

  private:
	shared_ptr<ISynthesizer> _synthesizer;
	shared_ptr<IEgg> _egg;
	double _timeToNextEvent;
	bool _noMoreNotes;
};

#endif