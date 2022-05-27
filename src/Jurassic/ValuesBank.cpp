#include "ValuesBank.hpp"
#include "Common/Capacities.hpp"

////////////////////////////////////////////////

ValuesBank::ValuesBank()
    : _constDoubles(CAPACITY_CONSTDOUBLES)
{}

bool ValuesBank::isConst(crealp x)
{
    return _constDoubles.contains(x);
}

////////////////////////////////////////////////

crealp ValuesBank::getConstant(double val)
{
    return _constDoubles.getOrAdd(val);
}

////////////////////////////////////////////////

ValuesBank g_valuesBank;