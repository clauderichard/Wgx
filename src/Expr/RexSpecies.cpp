
#include "RexSpecies.hpp"
#include "EquationSet.hpp"
#include "Jurassic/RealFunc.hpp"
#include "Jurassic/ITrex.hpp"
#include "Wavelang/TrexSpecies.hpp"
using namespace std;

////////////////////////////////////////////////

RexConst::RexConst(double val)
  : _val(val)
{
    _resultPlaceholder = g_valuesBank.getConstant(val);
}

RexConst::~RexConst() {}

crealp RexConst::processImpl(RexProcessContext &prog, Rex self)
{
    return _resultPlaceholder;
}

////////////////////////////////////////////////

RexVarName::RexVarName(const string &name)
    : _name(name) {}
    
RexVarName::~RexVarName() {}

crealp RexVarName::processImpl(RexProcessContext &prog, Rex self)
{
    return prog._eqns.get(_name).process(prog);
}

bool RexVarName::dependsOnInputTypeImpl(size_t type, RexProcessContext &prog, Rex self)
{
    if (!prog._eqns.contains(_name))
    {
        return true;
    }
    Rex rex = prog._eqns.get(_name);
    return rex.dependsOnInputType(type, prog, rex);
}

////////////////////////////////////////////////

RexInputVar::RexInputVar(size_t typeIndex, size_t index)
  : _typeIndex(typeIndex),
    _index(index) {}

RexInputVar::~RexInputVar() {}

crealp RexInputVar::processImpl(RexProcessContext &prog, Rex self)
{
    return _resultPlaceholder;
}

bool RexInputVar::dependsOnInputTypeImpl(size_t type, RexProcessContext &prog, Rex self)
{
    return _typeIndex == type;
}

////////////////////////////////////////////////

RexCustomFunArg::RexCustomFunArg(size_t index)
    : _index(index) {}
    
RexCustomFunArg::~RexCustomFunArg() {}

crealp RexCustomFunArg::processImpl(RexProcessContext &prog, Rex self)
{
    CRASH("RexCustomFunArg isn't supposed to be processed...")
}

////////////////////////////////////////////////

RexCustomFunCall::RexCustomFunCall(const string &funName, vector<Rex> args)
    : _funName(funName),
      _args(args) {}
RexCustomFunCall::~RexCustomFunCall() {}

Rex RexCustomFunCall::copyWithSubstitutionsImpl(RexSubstitutions &subs, Rex self)
{
    vector<Rex> newArgs;
    for(auto arg : _args)
    {
        newArgs.push_back(arg.copyWithSubstitutions(subs));
    }
    return Rex(new RexCustomFunCall(_funName, newArgs));
}

crealp RexCustomFunCall::processImpl(RexProcessContext &prog, Rex self)
{
    RexSubstitutions subs;
    for(size_t i=0; i<_args.size(); i++)
    {
        subs.set(Rex::getCustomFunParam(i), _args[i]);
    }
    Rex cop = prog._eqns.get(_funName).copyWithSubstitutions(subs);
    return cop.process(prog);
}

bool RexCustomFunCall::dependsOnInputTypeImpl(size_t type, RexProcessContext &prog, Rex self)
{
    for(auto &arg : _args)
    {
        if (arg.dependsOnInputType(type, prog, arg))
        {
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////

RexTrex::RexTrex(shared_ptr<ITrex> trex, vector<Rex> args)
    : _trex(trex),
      _args(args) {}

RexTrex::~RexTrex() {}

Rex RexTrex::copyWithSubstitutionsImpl(RexSubstitutions &subs, Rex self)
{
    auto newTrex = _trex->copy();
    RealpMap m;
    vector<Rex> newArgs;
    for(auto arg : _args)
    {
        auto newArg = arg.copyWithSubstitutions(subs);
        newArgs.push_back(newArg);
        m.set(arg.getResultPlaceholder(), newArg.getResultPlaceholder());
    }
    newTrex->updateArgs(m);
    shared_ptr<ITrex> newTrexShared(newTrex);
    return Rex(new RexTrex(newTrexShared, newArgs));
}

crealp RexTrex::processImpl(RexProcessContext &prog, Rex self)
{
    for(auto arg : _args)
    {
        arg.process(prog);
    }
    shared_ptr<ITrex> newTrexShared(_trex->copy());
    prog._resultTrexes.push_back(newTrexShared);
    return newTrexShared->getResult();
}

bool RexTrex::dependsOnInputTypeImpl(size_t type, RexProcessContext &prog, Rex self)
{
    for(auto &arg : _args)
    {
        if (arg.dependsOnInputType(type, prog, arg))
        {
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////

RexTrexNew::RexTrexNew(vector<Rex> args)
    : _args(args) {}

RexTrexNew::~RexTrexNew() {}

Rex RexTrexNew::copyWithSubstitutionsImpl(RexSubstitutions &subs, Rex self)
{
    vector<Rex> newArgs;
    for(auto arg : _args)
    {
        auto newArg = arg.copyWithSubstitutions(subs);
        newArgs.push_back(newArg);
    }
    return Rex(this->copy(newArgs));
}

crealp RexTrexNew::processImpl(RexProcessContext &prog, Rex self)
{
    for(auto arg : _args)
    {
        arg.process(prog);
    }
    shared_ptr<ITrex> newTrexShared(contructTrex());
    prog._resultTrexes.push_back(newTrexShared);
    return newTrexShared->getResult();
}

bool RexTrexNew::dependsOnInputTypeImpl(size_t type, RexProcessContext &prog, Rex self)
{
    for(auto &arg : _args)
    {
        if (arg.dependsOnInputType(type, prog, arg))
        {
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////

RexTrexFunc1::RexTrexFunc1(realfunc1 f, Rex a)
    : RexTrexNew({a}), _f(f) {}
RexTrexFunc1::~RexTrexFunc1() {}

ITrex *RexTrexFunc1::contructTrex()
{
    return new TrexFunc1(_f, _args[0].getResultPlaceholder());
}
RexTrexNew *RexTrexFunc1::copy(vector<Rex> &newArgs)
{
    return new RexTrexFunc1(_f, _args[0]);
}

////////////////////////////////////////////////
