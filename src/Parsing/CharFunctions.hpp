#ifndef __CHARFUNCTIONS_HPP__
#define __CHARFUNCTIONS_HPP__

#include <string>
using namespace std;

bool charIsDigit(char c);
bool charIsAlpha(char c);
bool charIsAlphaLower(char c);
bool charIsAlphaUpper(char c);
bool charIsAlphanumeric(char c);

double readReal(const string &s);

#endif