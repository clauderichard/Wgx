#ifndef __REALFUNC_HPP__
#define __REALFUNC_HPP__

#include <iostream>
using namespace std;

////////////////////////////////

typedef double (*realfunc1)(double);
typedef double (*realfunc2)(double,double);

////////////////////////////////
// Instances of realfuncs

double realfunc1_neg(double);
double realfunc1_inv(double);

double realfunc2_add(double, double);
double realfunc2_sub(double, double);
double realfunc2_mul(double, double);
double realfunc2_div(double, double);
double realfunc2_pow(double, double);

double realfunc2_gt(double, double);
double realfunc2_ge(double, double);
double realfunc2_lt(double, double);
double realfunc2_le(double, double);

////////////////////////////////

void printRealFunc(realfunc1 f);

void printRealFunc(realfunc2 f);

////////////////////////////////

#endif