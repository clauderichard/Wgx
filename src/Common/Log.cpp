#include "Log.hpp"

void printHexDigit(char h)
{
	if (h <= 9)
	{
		int a = h;
		cout << a;
	}
	else
	{
		int a = (h - 10) + 'A';
		cout << a;
	}
}
void printByte(char b)
{
	if (b >= '0' && b <= '9')
	{
		cout << b;
	}
	else if (b >= 'a' && b <= 'z')
	{
		cout << b;
	}
	else if (b >= 'A' && b <= 'Z')
	{
		cout << b;
	}
	else
	{
		int debugb = b & 0x0F;
		int debuga = (b & 0xF0) >> 4;
		cout << "x";
		printHexDigit(debuga);
		printHexDigit(debugb);
	}
}