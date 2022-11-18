#ifndef __VALUESBANK_HPP__
#define __VALUESBANK_HPP__

#include "Realp.hpp"
#include "ValuesSets.hpp"

////////////////////////////////////////////////

class ValuesBank
{
  public:
    ValuesBank();

    bool isConst(crealp);
    bool isChangingVar(crealp);

    crealp getConstant(double val);
    realp getChanging(const string &name);

  private:
    ConstsSet _constDoubles;
    ChangingValuesSet _changingDoubles;
};

////////////////////////////////////////////////

extern ValuesBank g_valuesBank;

////////////////////////////////////////////////

#endif