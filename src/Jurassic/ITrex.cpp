#include "ITrex.hpp"

ITrex::ITrex()
	: _result(0) {}

ITrex::~ITrex()
{
}

void ITrex::reset() {}

ITrex *ITrex::copyAndAddToMap(RealpMap &m)
{
	ITrex *ret = copy();
	ret->updateArgs(m);
	ret->initialize();
	m.set(&_result, &ret->_result);
	return ret;
}

void ITrex::updateArgs(RealpMap &m)
{
	auto args = getArgs();
	for (auto x : args)
	{
		*x = m[*x];
	}
}

crealp ITrex::getResult()
{
	return &_result;
}

bool ITrex::introducesChange() const
{
	return false;
}

void *ITrex::getResult() const
{
	return (void *)(&_result);
}

void ITrex::initialize() {}

void ITrex::print(CrealpNamer &n) const
{
	cout << "?";
}
