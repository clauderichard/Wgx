#ifndef __STATICBEZIERSPEC_HPP__
#define __STATICBEZIERSPEC_HPP__

////////////////////////////////////////////////

struct StaticBezierPoint
{
	double _x;
	double _y;
};

enum StaticBezierLinkType
{
	S_LINEAR = 0,
	S_QUADRATICFLATLEFT,
	S_QUADRATICFLATRIGHT,
	S_CUBICFLAT
};

////////////////////////////////////////////////

#endif