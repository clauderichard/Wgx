#ifndef __CREALPNAMER__HPP__
#define __CREALPNAMER__HPP__

#include "Common/Crash.hpp"
#include "Realp.hpp"
#include "ValuesSets.hpp"
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
		if (g_voiceInputVars.contains(x))
		{
			stringstream s;
			s << "v";
			s << 1;
			return s.str();
		}
		if (g_effectInputVars.contains(x))
		{
			stringstream s;
			s << "e";
			s << 1;
			return s.str();
			// return ret + "e";
		}
		if (g_constDoubles.contains(x))
		{
			stringstream s;
			//s << "c";
			s << *x;
			return s.str();
			// return "c";
		}
		if (g_namedVarsDoubles.contains(x))
		{
			return g_namedVarsDoubles.getNameOf(x);
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