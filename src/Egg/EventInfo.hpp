#ifndef __EVENTINFO_HPP__
#define __EVENTINFO_HPP__

#include <cstddef>
#include "Wave/StaticBezierSpec.hpp"
#include <vector>
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
	double *_varPtr;
	double _assignValue;
	vector<StaticBezierPoint> _bezierPoints;
	vector<StaticBezierLinkType> _bezierLinks;
	
	EggTime &refPostLag() { return _postLag; }
	EggTime &refDuration() { return _duration; }
	double &refPitch() { return _pitch; }
	size_t &refVoiceIndex() { return _voiceIndex; }
};

////////////////////////////////////////////////

#endif