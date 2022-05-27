
#include "EquationSet.hpp"
#include "Rex.hpp"
#include "RexProcessContext.hpp"
#include "Common/Crash.hpp"
#include "Jurassic/JurassicVoice.hpp"
#include "Jurassic/JurassicEffect.hpp"
#include <set>
using namespace std;

bool EquationSet::contains(const string &name)
{
    return _equations.find(name) != _equations.end();
}
void EquationSet::set(const string &name, Rex rex)
{
    _equations[name] = rex;
}
Rex EquationSet::get(const string &name)
{
    return _equations.at(name);
}

void EquationSet::addMissingFrom(EquationSet &other)
{
    for(auto &x : other._equations)
    {
        if (_equations.find(x.first) == _equations.end())
        {
            _equations[x.first] = x.second;
        }
    }
}
EquationSet EquationSet::copy() const
{
    EquationSet ret;
    for(auto &x : _equations)
    {
        ret._equations[x.first] = x.second;
    }
    return ret;
}

EquationSet EquationSet::operator+=(EquationSet &other)
{
    for(auto &x : other._equations)
    {
        _equations[x.first] = x.second;
    }
    return *this;
}

RexProcessContext EquationSet::process(vector<string> &outNames)
{
    RexProcessContext prog(*this);
    for(auto &outName : outNames)
    {
        if (_equations.find(outName) != _equations.end())
        {
            _equations.at(outName).process(prog);
        }
    }
    Validate(prog);
    prog.resetProcessedFlags();
    return prog;
}

void EquationSet::Validate(RexProcessContext &prog)
{
	std::set<crealp> newTrexResults;
	for(auto trex : prog._resultTrexes)
	{
		newTrexResults.insert(trex->getResult());
	}
	int numfdsaijf = 0;
	for(auto trex : prog._resultTrexes)
	{
		for(auto arg : trex->getArgs())
		{
            crealp newArg = prog._resultsMap[*arg];
            if (g_valuesBank.isConst(newArg))
                continue;
            bool isInput = false;
            for(size_t i=0; i<2; i++)
            {
                for(size_t j=0; j<2; j++)
                {
                    if (Rex::getInputVar(i,j).getResultPlaceholder() == newArg)
                    {
                        isInput = true;
                    }
                }
            }
            if (isInput)
                continue;
            if (newTrexResults.find(newArg) == newTrexResults.end())
            {
                CRASH("Old trex placeholder unmapped");
            }
		}
		numfdsaijf ++;
	}
}
bool EquationSet::isValidVoice()
{
    return !outDependsOnInputType(1);
}
bool EquationSet::isValidEffect()
{
    return !outDependsOnInputType(0);
}
shared_ptr<IVoice> EquationSet::toVoice()
{
    vector<string> outNames = getUsedOutNames();
    auto processResult = process(outNames);
    auto ret = make_shared<JurassicVoice>(2, processResult);
    return ret;
}
shared_ptr<IEffect> EquationSet::toEffect()
{
    vector<string> outNames = getUsedOutNames();
    auto processResult = process(outNames);
    auto ret = make_shared<JurassicEffect>(processResult);
    return ret;
}

bool EquationSet::outDependsOnInputType(size_t type)
{
    vector<string> outNames = getUsedOutNames();
    RexProcessContext prog(*this);
    for(auto s : outNames)
    {   
        if (_equations.find(s) != _equations.end())
        {
            Rex sx = _equations.at(s);
            if (sx.dependsOnInputType(1, prog, sx))
            {
                prog.resetProcessedFlags();
                return true;
            }
        }
    }
    prog.resetProcessedFlags();
    return false;
}

vector<string> EquationSet::getUsedOutNames()
{
    vector<string> ret;
    bool hasOutL = contains("outL");
    bool hasOutR = contains("outR");
    if (hasOutL && hasOutR)
    {
        return vector<string>{"outL","outR"};
    }
    if (hasOutL)
    {
        return vector<string>{"outL","out"};
    }
    if (hasOutR)
    {
        return vector<string>{"out","outR"};
    }
    return vector<string>{"out"};
}
