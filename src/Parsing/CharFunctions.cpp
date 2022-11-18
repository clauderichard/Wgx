#include "CharFunctions.hpp"
#include "Common/Keywords.hpp"

bool charIsDigit(char c)
{
	return c >= '0' && c <= '9';
}
bool charIsAlpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
bool charIsAlphaLower(char c)
{
	return (c >= 'a' && c <= 'z');
}
bool charIsAlphaUpper(char c)
{
	return (c >= 'A' && c <= 'Z');
}
bool charIsAlphanumeric(char c)
{
	return (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		(c >= '0' && c <= '9');
}
bool charIsGlobVarNameBegin(char c)
{
	return c == KEYWORD_GLOBVAR_BEGINCHAR;
}


double readReal(const string &s)
{
    return atof(s.c_str());
}