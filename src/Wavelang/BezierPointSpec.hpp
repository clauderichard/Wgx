#ifndef __BEZIERPOINTSPEC_hpp__
#define __BEZIERPOINTSPEC_hpp__

#include "Realp.hpp"
using namespace std;

////////////////////////////////////////////////

struct BezierPointSpec
{
	crealp _x;
	crealp _yLeft;
	crealp _yRight;
	crealp _dyLeft;
	crealp _dyRight;
};

struct BezierPointSpec0
{
	crealp _x;
	crealp _y;
};

////////////////////////////////////////////////

#endif