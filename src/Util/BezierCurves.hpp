#ifndef __BEZIERCURVES_HPP__
#define __BEZIERCURVES_HPP__

#include <exception>
#include <stdexcept>
using namespace std;

static inline double bezierInterpol0(
	double x1, double y1,
	double x2, double y2,
	double x)
{
	double t = (x - x1) / (x2 - x1);
	return t * y2 + (1 - t) * y1;
}

static inline double bezierInterpol10(
	double x1, double y1, double dy1,
	double x2, double y2,
	double x)
{
	double deltaX = x2 - x1;
	double df0 = dy1 * deltaX;
	double t = (x - x1) / deltaX;
	double s = 1 - t;
	return t * (t * y2 + s * df0) + s * (1 + t) * y1;
}

static inline double bezierInterpol01(
	double x1, double y1,
	double x2, double y2, double dy2,
	double x)
{
	double deltaX = x2 - x1;
	double df1 = dy2 * deltaX;
	double t = (x - x1) / deltaX;
	double s = 1 - t;
	return t * ((2 - t) * y2 - s * df1) + s * s * y1;
}

static inline double bezierInterpol11_flat(
	double x1, double y1,
	double x2, double y2,
	double x)
{
	// if x1,x2 = 0,1:
	//f(0) = A
	//f(1) = C
	//f'(0) = 0
	//f'(1) = 0
	//f(t) = t^2[ t C + (1-t) (3C) ]
	//     + (1-t)^2 [ t (3A) + (1-t) A ]
	//f(t) = t^2[ C(3-2t) ]
	//     + (1-t)^2 [ A(1+2t) ]
	//f(t) = C t^2 (3-2t)
	//     + A (1-t)^2 (1+2t)
	//f(t) = C t^2 (1+2s)
	//     + A s^2 (1+2t)

	double A = y1;
	double C = y2;
	double t = (x - x1) / (x2 - x1);
	double s = 1 - t;
	return C*t*t*(1 + 2*s) + A*s*s*(1 + 2*t);
}

static inline double bezierInterpol11(
	double x1, double y1, double dy1,
	double x2, double y2, double dy2,
	double x)
{
	// if x1,x2 = 0,1:
	//f(0) = A
	//f(1) = C
	//f'(0) = B
	//f'(1) = D
	//f(t) = t^2[ t C + (1-t) (3C-D) ]
	//     + (1-t)^2 [ t (3A+B) + (1-t) A ]

	double deltaX = x2 - x1;
	double A = y1;
	double B = dy1 * deltaX;
	double C = y2;
	double D = dy2 * deltaX;
	double t = (x - x1) / deltaX;
	double s = 1 - t;
	return t * t * (t * C + s * (3 * C - D)) + s * s * (t * (3 * A + B) + s * A);
}

#endif