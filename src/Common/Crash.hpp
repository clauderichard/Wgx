#ifndef __CRASH_HPP__
#define __CRASH_HPP__

#include <iostream>
using namespace std;

#define CRASH(x) \
    cout << x << endl; \
    throw 9999;

#define NOTIMPL(x) \
    cout << x << " not implemented" << endl; \
    throw 9999;

#endif