
#include "RealpMap.hpp"
#include <iostream>
#include <stdexcept>
using namespace std;

crealp RealpMap::operator[](crealp x)
{
	try
	{
		auto mx = _realMap.at(x);
		return mx;
	}
	catch (out_of_range)
	{
		return x;
	}
}

void RealpMap::set(crealp x, crealp y)
{
	_realMap[x] = y;
}

void RealpMap::print() const
{
	for (auto x : _realMap)
	{
		cout << x.first << " -> " << x.second << endl;
	}
}