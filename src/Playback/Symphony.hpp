#ifndef __SYMPHONY_HPP__
#define __SYMPHONY_HPP__

#include "Wave/IWaveSampler.hpp"
#include "Wave/Synthesizer.hpp"
#include "Egg/IEgg.hpp"
#include "Common/Crash.hpp"
#include "Common/SampleRate.hpp"
#include <iostream>
using namespace std;

class Symphony : public IWaveSampler
{
  public:
	Symphony(shared_ptr<Synthesizer> synth, shared_ptr<IEgg> gen)
		: _synthesizer(synth),
		  _generator(gen),
		  _timeToNextEvent(0),
		  _noMoreNotes(false) {}
	~Symphony()
	{
	}

	void genSample()
	{
		if (!_noMoreNotes)
		{
			_timeToNextEvent -= 1.0 / WGX_SAMPLESPERSECOND;
			while (_timeToNextEvent <= 0)
			{
				Event next;
				try
				{
					next = _generator->pop();
				}
				catch (bool)
				{
					_noMoreNotes = true;
					break;
				}

				next._action->execute(next._info);

				_timeToNextEvent = next._info._postLag;
			}
		}
		_synthesizer->genSample();
		_left = _synthesizer->_left;
		_right = _synthesizer->_right;
	}

  private:
	shared_ptr<Synthesizer> _synthesizer;
	shared_ptr<IEgg> _generator;
	double _timeToNextEvent;
	bool _noMoreNotes;
};

#endif