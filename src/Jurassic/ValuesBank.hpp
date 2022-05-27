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

    crealp getConstant(double val);

  private:
    ConstsSet _constDoubles;
};

////////////////////////////////////////////////

extern ValuesBank g_valuesBank;

////////////////////////////////////////////////

#endif