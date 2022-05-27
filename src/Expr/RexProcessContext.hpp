#ifndef __REXPROCESSCONTEXT_HPP__
#define __REXPROCESSCONTEXT_HPP__

#include <vector>
#include <memory>
#include "Jurassic/RealpMap.hpp"
using namespace std;

class ITrex;
class EquationSet;
class IRexImpl;
struct RexSubstitutions;
class Rex;

////////////////////////////////////////////////

struct RexProcessContext
{
    EquationSet &_eqns;
    vector<shared_ptr<ITrex>> _resultTrexes;
    RealpMap _resultsMap;
    vector<Rex> _processedRexes;

    RexProcessContext(EquationSet &eqns);

    void flagProcessed(Rex processed);
    void resetProcessedFlags();
    
};

////////////////////////////////////////////////

#endif