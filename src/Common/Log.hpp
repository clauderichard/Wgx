#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <iostream>
using namespace std;

void printHexDigit(char h);
void printByte(char b);

#ifdef DEBUG_LOGS

#define logAddr(x)                       \
	cout << "&" << #x << " =" << &(x) << endl;

#define logVal(x)                       \
	cout << #x << " = " << x << endl;

#define logString(x)                       \
	cout << x << endl;

#define logByteArray(x, siz)                   \
	cout << #x << " (" << &(x) << ")";           \
	cout << " = ";                               \
	for (int debugi = 0; debugi < siz; debugi++) \
	{                                            \
		printByte(x[debugi]);                    \
	}                                            \
	cout << endl;

#else

// DEBUG_LOGS absent so macros don't do anything
#define logAddr(x)
#define logVal(x)
#define logByteArray(x, siz)
#define logString(x)

#endif

#endif