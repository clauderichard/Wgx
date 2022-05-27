#include "Rex.hpp"
#include "RexProcessContext.hpp"
#include "Jurassic/ITrex.hpp"

////////////////////////////////////////////////

RexProcessContext::RexProcessContext(EquationSet &eqns)
    : _eqns(eqns) {}
    
void RexProcessContext::resetProcessedFlags()
{
    for(auto x : _processedRexes)
    {
        x._impl->_processed = false;
    }
}
void RexProcessContext::flagProcessed(Rex processed)
{
    _processedRexes.push_back(processed);
    processed._impl->_processed = true;
}
    
////////////////////////////////////////////////
