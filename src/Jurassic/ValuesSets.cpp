#include "ValuesSets.hpp"
#include "Common/Capacities.hpp"
#include "Common/Crash.hpp"
#include <iostream>
using namespace std;

////////////////////////////////////////////////

ValuesSet::ValuesSet(size_t capacity)
	: _vals(capacity) {}

bool ValuesSet::contains(const double *addr) const
{
	const double *a = &(_vals[0]);
	const double *b = &(_vals[0]) + _vals.size();
	return a <= addr && addr < b;
}

void ValuesSet::print()
{
	cout << "ValuesSet (" << &_vals[0];
	cout << " -> " << &_vals[_vals.size()-1];
	cout << ")" << endl;
}

////////////////////////////////////////////////

ConstsSet::ConstsSet(size_t capacity)
	: ValuesSet(capacity),
	  _numVals(0) {}

const double *ConstsSet::getOrAdd(const double &val)
{
	try
	{
		size_t i = _valToIndex.at(val);
		return &(_vals[i]);
	}
	catch (out_of_range)
	{
		if (_numVals >= _vals.size())
		{
			throw out_of_range("Too many entries in this registry");
		}
		_vals[_numVals] = val;
		_valToIndex[val] = _numVals;
		return &(_vals[_numVals++]);
	}
}

////////////////////////////////////////////////

ChangingValuesSet::ChangingValuesSet(size_t capacity)
	: ValuesSet(capacity),
	  _numVals(0) {}

double *ChangingValuesSet::getOrAdd(const string &name)
{
	try
	{
		size_t i = _nameToIndex.at(name);
		return &(_vals[i]);
	}
	catch (out_of_range)
	{
		if (_numVals >= _vals.size())
		{
			throw out_of_range("Too many entries in this registry");
		}
		_vals[_numVals] = 0;
		_nameToIndex[name] = _numVals;
		return &(_vals[_numVals++]);
	}
}

////////////////////////////////////////////////
