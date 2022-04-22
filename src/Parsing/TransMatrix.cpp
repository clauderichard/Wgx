
#include "Parsing/TransMatrix.hpp"
#include "Common/All.hpp"
#include <map>
#include <iostream>

TransMatrix::TransMatrix(bool hasI)
	: _hasI(hasI)
{
}
TransMatrix::~TransMatrix() {}

bool TransMatrix::hasI()
{
	return _hasI;
}
void TransMatrix::addArrow(TkType a, TkType b)
{
	getBs(a).insert(b);
}
bool TransMatrix::hasArrow(TkType a, TkType b) const
{
	if (a==b && _hasI)
	{
		return true;
	}
	try
	{
		auto &bs = _map.at(a);
		return bs.find(b) != bs.end();
	}
	catch(const std::exception &)
	{
		return false;
	}
}

set<TkType> &TransMatrix::getBs(TkType a)
{
	try
	{
		return _map.at(a);
	}
	catch (out_of_range)
	{
		return _map[a];
	}
}

bool TransMatrix::operator==(TransMatrix &other) const
{
	return (_hasI == other._hasI) && (_map == other._map);
}

const set<TkType> &TransMatrix::operator[](TkType a)
{
	return getBs(a);
}

TransMatrix TransMatrix::operator*(TransMatrix &other)
{
	TransMatrix ret(_hasI && other._hasI);
	if (_hasI)
	{
		for (auto ys : other._map)
		{
			ret.getBs(ys.first).insert(
				ys.second.begin(),
				ys.second.end());
		}
	}
	if (other._hasI)
	{
		for (auto xs : _map)
		{
			ret.getBs(xs.first).insert(
				xs.second.begin(),
				xs.second.end());
		}
	}
	for (auto xs : _map)
	{
		for (auto y : xs.second)
		{
			try
			{
				auto zs = other._map.at(y);
				ret.getBs(xs.first).insert(
					zs.begin(), zs.end());
			}
			catch (out_of_range)
			{
			}
		}
	}
	return ret;
}

size_t TransMatrix::getNumNonzeroes() const
{
	size_t ret = 0;
	for(auto x : _map)
	{
		ret += x.second.size();
	}
	return ret;
}

TransMatrix TransMatrix::selfSquare()
{
	if (!_hasI)
	{
		CRASH("TransMatrix::selfSquare not implemented if this does not include I")
	}
	TransMatrix ret(_hasI);
	for (auto xs : _map)
	{
		ret.getBs(xs.first).insert(
			xs.second.begin(),
			xs.second.end());
	}
	for (auto xs : _map)
	{
		for (auto y : xs.second)
		{
			try
			{
				auto zs = _map.at(y);
				ret.getBs(xs.first).insert(
					zs.begin(), zs.end());
			}
			catch (out_of_range)
			{
			}
		}
	}
	return ret;
}

void TransMatrix::selfPowInf()
{
	if (!_hasI)
	{
		CRASH("TransMatrix::selfPowInf impossible if this does not include I")
	}

	size_t numNonZeroes = getNumNonzeroes();
	size_t xNonZeroes;
	while (true)
	{
		TransMatrix x = selfSquare();
		xNonZeroes = x.getNumNonzeroes();
		if (xNonZeroes == numNonZeroes)
		{
			break;
		}
		_map = x._map;
		numNonZeroes = xNonZeroes;
	}
}
