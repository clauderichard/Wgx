#ifndef __TOKEN_HPP__
#define __TOKEN_HPP__

#include <cstddef>
#include <string>
#include <memory>
#include "TypeDefs.hpp"
using namespace std;

struct Token
{
	TkType _type;
	shared_ptr<string> _string;

	// where in the code string is this
	size_t _lineNumber;
	size_t _columnNumber;
};

#endif