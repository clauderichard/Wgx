#include "WaveLang.hpp"
#include "Parsing/CharFunctions.hpp"
#include "Jurassic/JurassicUniverse.hpp"
#include "Jurassic/JurassicVoice.hpp"
#include "Jurassic/ValuesBank.hpp"
#include "Jurassic/RealFunc.hpp"
#include "TrexSpecies.hpp"
#include "Expr/RexSpecies.hpp"
#include <sstream>
using namespace std;

shared_ptr<JurassicUniverse> g_jUniverse;

////////////////////////////////////////////////
// Traverser pre-functions
// (call before traversing children)

void newJUniverse()
{
	g_jUniverse.reset(new JurassicUniverse());
}

////////////////////////////////////////////////
// Atoms

double realFromInt(int &a)
{
	return a * 1.0;
}
Rex rexFromReal(double &a)
{
	return Rex::getConstant(a);
}

Rex rexVarFromName(string &name)
{
	return Rex::getNamedVar(name);
}

////////////////////////////////////////////////
// Bezier

struct BezierPointEx
{
	Rex _x;
	Rex _y;
};

struct BezierCurveEx
{
	vector<BezierPointEx> _pts;
	vector<BezierLinkType> _links;
};

BezierPointEx wBezierPtExBuild(Rex &x, Rex &y)
{
	BezierPointEx pt;
	pt._x = x;
	pt._y = y;
	return pt;
}

BezierCurveEx wBezierCurveExBuild(
	vector<BezierPointEx> &pts,
	vector<BezierLinkType> &links)
{
	BezierCurveEx curve;
	curve._pts = pts;
	curve._links = links;
	return curve;
}

////////////////////////////////////////////////

Rex wRexTrex_helper(ITrex *trex, vector<Rex> args)
{
	shared_ptr<ITrex> trexShared(trex);
    return Rex(new RexTrex(trexShared, args));
}

Rex wBezierWaveExBuild(
	Rex &x,
	BezierCurveEx &curve)
{
	vector<BezierPointSpec0> newPts;
	vector<Rex> args{x};
	for(auto &pt : curve._pts)
	{
		BezierPointSpec0 newPt;
		newPt._x = pt._x.getResultPlaceholder();
		newPt._y = pt._y.getResultPlaceholder();
		newPts.push_back(newPt);
		args.push_back(pt._x);
		args.push_back(pt._y);
	}
	return wRexTrex_helper(new TrexBezierCurves0(
		newPts, curve._links, x.getResultPlaceholder()), args);
}

Rex wApplyUnopRex_helper(realfunc1 f, Rex &b)
{
	return wRexTrex_helper(new TrexFunc1(f, b.getResultPlaceholder()), {b});
}
Rex wApplyBinopRex_helper(realfunc2 g, Rex &a, Rex &b)
{
	return wRexTrex_helper(new TrexFunc2(g,
		a.getResultPlaceholder(),
		b.getResultPlaceholder()), {a,b});
}
Rex wApplyUnopRex(Op1 &o, Rex &b)
{
	return wApplyUnopRex_helper(o._f, b);
}
Rex wApplyBinopRex(Rex &a, Op2 &o, Rex &b)
{
	return wApplyBinopRex_helper(o._g, a, b);
}
Rex wApplyAmbop1Rex(OpAmbiguous &o, Rex &b)
{
	return wApplyUnopRex_helper(o._f, b);
}
Rex wApplyAmbop2Rex(Rex &a, OpAmbiguous &o, Rex &b)
{
	return wApplyBinopRex_helper(o._g, a, b);
}

Rex wRandomRex(Rex &a, Rex &b) {
	return wRexTrex_helper(new TrexRandom(
		a.getResultPlaceholder(),
		b.getResultPlaceholder()), {a,b});
}
Rex wPrevRex(Rex &a) {
	return wRexTrex_helper(new TrexPrev(
		a.getResultPlaceholder()), {a});
}
Rex wOnLeftRex(Rex &a, Rex &b) {
	return wRexTrex_helper(new TrexChangeWhenLeft(
		a.getResultPlaceholder(),
		b.getResultPlaceholder()), {a,b});
}
Rex wTimerRex() { return wRexTrex_helper(new TrexTimer(), {}); }
Rex wPhaseRex(Rex &freq)
{
	return wRexTrex_helper(new TrexPhase(
		freq.getResultPlaceholder()), {freq});
}
Rex wDelayRex(Rex &delay, Rex &inp)
{
	return wRexTrex_helper(new TrexDelay(
		inp.getResultPlaceholder(),
		delay.getResultPlaceholder()), {inp,delay});
}

////////////////////////////////////////////////

pair<string, Rex> wEquation(string &name, Rex &rex)
{
	return pair<string, Rex>(name, rex);
}

pair<string, vector<Rex>> wCustomFunctionArgs(string &name, vector<Rex> &args)
{
	return pair<string, vector<Rex>>(name, args);
}

pair<string, Rex> makeCustomFunctionDefinition(pair<string, vector<Rex>> &nameAndArgs, Rex &rex)
{
	vector<Rex> &args = nameAndArgs.second;
	// replace rex's namedVars corresponding to args, with arg placeholders.
	RexSubstitutions subs;
	for(size_t i = 0; i < args.size(); i ++)
	{
		subs.set(args[i], Rex::getCustomFunParam(i));
	}
	Rex newR = rex.copyWithSubstitutions(subs);
	return pair<string, Rex>(nameAndArgs.first, newR);
}

Rex makeCustomFunctionCallRex(pair<string, vector<Rex>> &nameWithArgs)
{
	return Rex(new RexCustomFunCall(nameWithArgs.first, nameWithArgs.second));
}

shared_ptr<EquationSet> equationSetFromParkBindings(vector<pair<string, Rex>> &bindings)
{
	shared_ptr<EquationSet> eqns = make_shared<EquationSet>();
	for (size_t i = 0; i < bindings.size(); i++)
	{
		eqns->set(bindings[i].first, bindings[i].second);
	}
	return eqns;
}

////////////////////////////////////////////////

shared_ptr<EquationSet> jurassicWorldFromJName(string &jname)
{
	return g_jUniverse->getWorld(jname);
}

int jurassicWorldBindingSimple(string &name, shared_ptr<EquationSet> &world)
{
	g_jUniverse->addWorld(name, world);
	return 7;
}
int jurassicWorldBindingVoice(string &name, shared_ptr<EquationSet> &world)
{
	g_jUniverse->addWorld(name, world);
	g_jUniverse->setNameIsVoice(name);
	return 7;
}
int jurassicWorldBindingEffect(string &name, shared_ptr<EquationSet> &world)
{
	g_jUniverse->addWorld(name, world);
	g_jUniverse->setNameIsEffect(name);
	return 7;
}

shared_ptr<EquationSet> jworldFrom2Jworlds(
	shared_ptr<EquationSet> &a,
	shared_ptr<EquationSet> &b)
{
	shared_ptr<EquationSet> c = make_shared<EquationSet>();
	c->addMissingFrom(*b);
	c->addMissingFrom(*a);
	return c;
}

////////////////////////////////////////////////

int connectionFromNames(string &inp, string &outp)
{
	g_jUniverse->addConnection(inp, outp);
	return 9;
}

////////////////////////////////////////////////

WaveLangResult wUniverseFromNamedWorlds(
	vector<int> &worldBindings)
{
	return g_jUniverse->toSynthesizer();
}

////////////////////////////////////////////////
