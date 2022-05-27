#include "Rex.hpp"
#include "Jurassic/ITrex.hpp"
#include "RexSpecies.hpp"

////////////////////////////////////////////////

void RexSubstitutions::set(Rex x, Rex newX)
{
	_subs[x.getResultPlaceholder()] = newX;
    // _substitutions.emplace_back(x,newX);
}

Rex RexSubstitutions::get(Rex x)
{
    crealp res = x.getResultPlaceholder();
	if (_subs.find(res) != _subs.end())
	{
		return _subs[res];
	}
	else
	{
		return x;
	}
    // for(auto &sub : _substitutions)
    // {
    //     if (sub.first._impl.get() == x._impl.get())
    //     {
    //         return sub.second;
    //     }
    // }
    // return x;
}

////////////////////////////////////////////////

Rex::Rex()
    : _impl(NULL) {}

Rex::Rex(IRexImpl *impl)
    : _impl(impl) {}
// Rex::Rex(shared_ptr<IRexImpl> impl)
//     : _impl(impl) {}

Rex::Rex(const Rex &other)
    : _impl(other._impl) {}

Rex Rex::copyWithSubstitutions(RexSubstitutions &subs)
{
    return _impl->copyWithSubstitutions(subs, *this);
}

crealp Rex::process(RexProcessContext &prog)
{
    return _impl->process(prog, *this);
}

crealp Rex::getResultPlaceholder()
{
    return _impl->_resultPlaceholder;
}

Rex Rex::getConstant(double val)
{
    return Rex(new RexConst(val));
}
Rex Rex::getCustomFunParam(size_t index)
{
    while(index >= g_customFunParams.size())
    {
        g_customFunParams.push_back(Rex(new RexCustomFunArg(g_customFunParams.size())));
    }
    return g_customFunParams[index];
}
Rex Rex::getInputVar(size_t typeIndex, size_t index)
{
    while(typeIndex >= g_inputVars.size())
    {
        g_inputVars.emplace_back();
    }
    while(index >= g_inputVars[typeIndex].size())
    {
        g_inputVars[typeIndex].push_back(Rex(new RexInputVar(typeIndex, g_inputVars[typeIndex].size())));
    }
    return g_inputVars[typeIndex][index];
}
Rex Rex::getNamedVar(const string &name)
{
    if (g_namedVars.find(name) == g_namedVars.end())
    {
        g_namedVars[name] = Rex(new RexVarName(name));
    }
    return g_namedVars[name];
}
crealp Rex::getNewResultPlaceholder()
{
    if (g_resultPlaceholdersCount >= 2048)
    {
        CRASH("Too many result placeholders used!");
    }
    return &g_resultPlaceholders[g_resultPlaceholdersCount++];
}

bool Rex::dependsOnInputType(size_t type, RexProcessContext &prog, Rex self)
{
    return _impl->dependsOnInputType(type, prog, self);
}

vector<Rex> Rex::g_customFunParams;
vector<vector<Rex>> Rex::g_inputVars;
map<string,Rex> Rex::g_namedVars;
vector<double> Rex::g_resultPlaceholders(2048);
size_t Rex::g_resultPlaceholdersCount = 0;

////////////////////////////////////////////////

IRexImpl::IRexImpl()
    : _processed(false),
      _resultPlaceholder(Rex::getNewResultPlaceholder()) {}

IRexImpl::~IRexImpl() {}

Rex IRexImpl::copyWithSubstitutions(RexSubstitutions &subs, Rex self)
{
    Rex x = subs.get(self);
    if (self._impl.get() != x._impl.get())
    {
        return x;
    }
    else
    {
        return copyWithSubstitutionsImpl(subs, self);
    }
}

Rex IRexImpl::copyWithSubstitutionsImpl(RexSubstitutions &subs, Rex self)
{
    return self;
}

crealp IRexImpl::process(RexProcessContext &prog, Rex self)
{
    if (!_processed)
    {
        prog.flagProcessed(self);
        crealp newResult = processImpl(prog, self);
        prog._resultsMap.set(_resultPlaceholder, newResult);
        return newResult;
    }
    return prog._resultsMap[_resultPlaceholder];
}

bool IRexImpl::dependsOnInputType(size_t type, RexProcessContext &prog, Rex self)
{
    if (!_processed)
    {
        prog.flagProcessed(self);
        return dependsOnInputTypeImpl(type, prog, self);
    }
    else
    {
        return false;
    }
}

bool IRexImpl::dependsOnInputTypeImpl(size_t type, RexProcessContext &prog, Rex self)
{
    return false;
}

////////////////////////////////////////////////
