
#include "Symphony.hpp"
#include "Egglang/EggExceptions.hpp"
#include <iostream>
using namespace std;

Symphony::Symphony(shared_ptr<ISynthesizer> synth, shared_ptr<IEgg> egg)
	: _synthesizer(synth),
		_egg(egg),
		_timeToNextEvent(0),
		_noMoreNotes(false) {}

Symphony::~Symphony() {}

void Symphony::genSample()
{
	if (!_noMoreNotes)
	{
		_timeToNextEvent -= 1.0 / WGX_SAMPLESPERSECOND;
		while (_timeToNextEvent <= 0)
		{
			Event next;
			try
			{
				next = _egg->pop();
			}
			catch (OfficialEnd)
			{
				_noMoreNotes = true;
				break;
			}
			catch (NoEventsLeft)
			{
				_noMoreNotes = true;
				break;
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
