#ifndef __EVENTINFO_HPP__
#define __EVENTINFO_HPP__

#include <cstddef>
using namespace std;

////////////////////////////////////////////////

typedef double GenTime;

////////////////////////////////////////////////

struct EventInfo
{
	GenTime _postLag;
	GenTime _duration;
	double _pitch;
	size_t _voiceIndex;
	bool _isStart;

	GenTime &refPostLag() { return _postLag; }
	GenTime &refDuration() { return _duration; }
	double &refPitch() { return _pitch; }
	size_t &refVoiceIndex() { return _voiceIndex; }
};

////////////////////////////////////////////////

#endif