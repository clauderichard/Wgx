#ifndef __REX_HPP__
#define __REX_HPP__

#include <vector>
#include <memory>
#include <string>
#include "Jurassic/RealpMap.hpp"
#include "Jurassic/RealFunc.hpp"
#include "RexProcessContext.hpp"
using namespace std;

class ITrex;
class IRexImpl;
class EquationSet;
struct RexSubstitutions;

////////////////////////////////////////////////

class Rex
{
  public:
    shared_ptr<IRexImpl> _impl;

    Rex();
    Rex(IRexImpl *impl);
    Rex(const Rex &other);

    Rex copyWithSubstitutions(RexSubstitutions &subs);
    crealp process(RexProcessContext &prog);
    crealp getResultPlaceholder();

    static Rex getConstant(double val);
    static Rex getCustomFunParam(size_t index);
    static Rex getInputVar(size_t typeIndex, size_t index);
    static Rex getNamedVar(const string &name);
    static crealp getNewResultPlaceholder();

    bool dependsOnInputType(size_t type, RexProcessContext &prog, Rex self);
    
    static vector<Rex> g_customFunParams;
    static vector<vector<Rex>> g_inputVars;
    static map<string,Rex> g_namedVars;
    static vector<double> g_resultPlaceholders;
    static size_t g_resultPlaceholdersCount;

};

////////////////////////////////////////////////

class IRexImpl
{
  public:
    IRexImpl();
    virtual ~IRexImpl();

    Rex copyWithSubstitutions(RexSubstitutions &subs, Rex self);
    virtual Rex copyWithSubstitutionsImpl(RexSubstitutions &subs, Rex self);

    crealp process(RexProcessContext &prog, Rex self);
    virtual crealp processImpl(RexProcessContext &prog, Rex self) = 0;
    
    bool dependsOnInputType(size_t type, RexProcessContext &prog, Rex self);
    virtual bool dependsOnInputTypeImpl(size_t type, RexProcessContext &prog, Rex self);

    crealp _resultPlaceholder;

  protected:
    bool _processed;

    friend struct RexProcessContext;

};

////////////////////////////////////////////////

struct RexSubstitutions
{
    void set(Rex x, Rex newX);
    Rex get(Rex x);

    map<crealp,Rex> _subs;
};

////////////////////////////////////////////////

#endif