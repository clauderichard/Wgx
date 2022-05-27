#ifndef __EVENTINFO_HPP__
#define __EVENTINFO_HPP__

#include <cstddef>
using namespace std;

////////////////////////////////////////////////

typedef double EggTime;

////////////////////////////////////////////////

struct EventInfo
{
	EggTime _postLag;
	EggTime _duration;
	double _pitch;
	size_t _voiceIndex;
	bool _isStart;

	EggTime &refPostLag() { return _postLag; }
	EggTime &refDuration() { return _duration; }
	double &refPitch() { return _pitch; }
	size_t &refVoiceIndex() { return _voiceIndex; }
};

////////////////////////////////////////////////

#endif