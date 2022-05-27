#ifndef __CREALPNAMER__HPP__
#define __CREALPNAMER__HPP__

#include "Common/Crash.hpp"
#include "Realp.hpp"
#include "ValuesBank.hpp"
#include <map>
#include <string>
#include <sstream>
using namespace std;

////////////////////////////////////////////////

class CrealpNamer
{
	map<crealp, string> _names;
	char _nextLetter;

  public:
    CrealpNamer()
		: _nextLetter('A') {}

	void addName(crealp x)
	{
		if (_names.find(x) != _names.end())
		{
			CRASH("name already added");
		}
		_names[x] = _nextLetter;
		_nextLetter ++;
	}

	void addName(crealp x, const string &name)
	{
		if (_names.find(x) != _names.end())
		{
			CRASH("name already added");
		}
		_names[x] = name;
	}

	string operator[](crealp x)
	{
		string ret = "";
		if (g_valuesBank.isConst(x))
		{
			stringstream s;
			//s << "c";
			s << *x;
			return s.str();
			// return "c";
		}
		try
		{
			return _names.at(x);
		}
		catch(const std::exception&)
		{
			stringstream s;
			s << x;
			return s.str();
		}
		
	}
};

////////////////////////////////////////////////

#endif