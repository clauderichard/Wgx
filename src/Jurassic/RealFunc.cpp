#define _USE_MATH_DEFINES
#include <cmath>

#include "RealFunc.hpp"
using namespace std;

////////////////////////////////////////////////

const double g_math_pi = 3.14159265358979323846;
const double g_math_2pi = 2*g_math_pi;

////////////////////////////////////////////////

double realfunc1_neg(double x) { return -x; }
double realfunc1_inv(double x) { return 1 / x; }

////////////////////////////////////////////////

double realfunc2_add(double a, double b) { return a + b; }
double realfunc2_sub(double a, double b) { return a - b; }
double realfunc2_mul(double a, double b) { return a * b; }
double realfunc2_div(double a, double b) { return a / b; }
double realfunc2_pow(double a, double b)
{
	if (a < 0)
	{
		return -std::pow(-a, b);
	}
	return std::pow(a, b);
}

////////////////////////////////////////////////

double realfunc2_gt(double a, double b) { return a > b ? 1 : -1; }
double realfunc2_ge(double a, double b) { return a >= b ? 1 : -1; }
double realfunc2_lt(double a, double b) { return a < b ? 1 : -1; }
double realfunc2_le(double a, double b) { return a <= b ? 1 : -1; }

////////////////////////////////////////////////

void printRealFunc(realfunc1 f)
{
    if (f == realfunc1_neg)
    {
        cout << "-";
    }
    else if (f == realfunc1_inv)
    {
        cout << "1/";
    }
    else
    {
        cout << "f1";
    }
}

void printRealFunc(realfunc2 f)
{
    if (f == realfunc2_add)
    {
        cout << "+";
    }
    else if (f == realfunc2_sub)
    {
        cout << "-";
    }
    else if (f == realfunc2_mul)
    {
        cout << "*";
    }
    else if (f == realfunc2_div)
    {
        cout << "/";
    }
    else if (f == realfunc2_pow)
    {
        cout << "**";
    }
    else
    {
        cout << "f2";
    }
}

////////////////////////////////////////////////
