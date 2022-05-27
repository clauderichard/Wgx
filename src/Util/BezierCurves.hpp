#ifndef __BEZIERCURVES_HPP__
#define __BEZIERCURVES_HPP__

#include <exception>
#include <stdexcept>
using namespace std;

////////////////////////////////////////////////

static inline double bezierInterpol_linear(
	double x1, double y1,
	double x2, double y2,
	double x)
{
	double t = (x - x1) / (x2 - x1);
	return t * y2 + (1 - t) * y1;
}

static inline double bezierInterpol_quadraticLeftFlat(
	double x1, double y1,
	double x2, double y2,
	double x)
{
	// Quadratic centered at (x1,y1) going through (x2,y2)
	// f(t) = y1 + A(x-x1)^2

	// f(x2) = y2
	// y1 + A(x2-x1)^2 = y2
	// A = (y2-y1) / (x2-x1)^2
	// f(t) = y1 + (y2-y1)(x-x1)^2 / (x2-x1)^2

	double dx1 = x - x1;
	double dx = x2 - x1;
	double dx1overdx = dx1 / dx;
	return y1 + (y2-y1)*dx1overdx*dx1overdx;
}

static inline double bezierInterpol_cubicflat(
	double x1, double y1,
	double x2, double y2,
	double x)
{
	// Cubic with derivative 0 at (x1,y1) and (x2,y2)
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

////////////////////////////////////////////////

#endif