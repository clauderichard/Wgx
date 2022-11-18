#include "ValuesBank.hpp"
#include "Common/Capacities.hpp"

////////////////////////////////////////////////

ValuesBank::ValuesBank()
    : _constDoubles(CAPACITY_CONSTDOUBLES),
      _changingDoubles(CAPACITY_CHANGINGDOUBLES)
{}

bool ValuesBank::isConst(crealp x)
{
    return _constDoubles.contains(x);
}

bool ValuesBank::isChangingVar(crealp x)
{
    return _changingDoubles.contains(x);
}

////////////////////////////////////////////////

crealp ValuesBank::getConstant(double val)
{
    return _constDoubles.getOrAdd(val);
}

realp ValuesBank::getChanging(const string &name)
{
    return _changingDoubles.getOrAdd(name);
}

////////////////////////////////////////////////

ValuesBank g_valuesBank;