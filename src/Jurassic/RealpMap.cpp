
#include "RealpMap.hpp"
#include <iostream>
#include <stdexcept>
using namespace std;

crealp RealpMap::operator[](crealp x)
{
	crealp ret = x;
	while(true)
	{
		try
		{
			ret = _realMap.at(ret);
		}
		catch (out_of_range)
		{
			return ret;
		}
	}
}

void RealpMap::set(crealp x, crealp y)
{
	if (x != y)
	{
		_realMap[x] = y;
	}
}

void RealpMap::flagAsChanging(crealp x)
{
	_changingValues.insert(x);
}

bool RealpMap::isChanging(crealp x)
{
	if(_changingValues.find(x) != _changingValues.end())
	{
		return true;
	}
	crealp y = operator[](x);
	return _changingValues.find(y) != _changingValues.end();
}

void RealpMap::print() const
{
	for (auto x : _realMap)
	{
		cout << x.first << " -> " << x.second << endl;
	}
}