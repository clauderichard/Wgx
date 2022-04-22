#include "WaveLang.hpp"
#include "Parsing/CharFunctions.hpp"
#include "JurassicVoice.hpp"
#include "Wave/IVoice.hpp"
#include "JurassicWorld.hpp"
#include "JurassicUniverse.hpp"
#include "Rex.hpp"
#include "RealFunc.hpp"
#include "ValuesSets.hpp"
#include "TrexSpecies.hpp"
#include "TrexSpeciesEffects.hpp"
#include <sstream>
using namespace std;

shared_ptr<JurassicUniverse> g_jUniverse;
shared_ptr<JurassicWorld> defaultWorld;

////////////////////////////////////////////////
// Traverser pre-functions
// (call before traversing children)

void newJPark()
{
	Rex::useNewPark();
}

void newJUniverse()
{
	g_jUniverse.reset(new JurassicUniverse());
}

////////////////////////////////////////////////

struct BezierCurve
{
	vector<BezierPointSpec0> _pts;
	vector<bool> _links;
};

////////////////////////////////////////////////
// Atoms

double realFromInt(int &a)
{
	return a * 1.0;
}
crealp rexFromReal(double &a)
{
	return g_constDoubles.getOrAdd(a);
}

crealp rexVarFromName(string &name)
{
	return g_namedVarsDoubles.getOrAdd(name);
}

////////////////////////////////////////////////
// Bezier new

BezierPointSpec0 wBezierPt0FromRex2(crealp &x, crealp &y)
{
	BezierPointSpec0 pt;
	pt._x = x;
	pt._y = y;
	return pt;
}

BezierCurve wBezierCurveFrom2Vecs(
	vector<BezierPointSpec0> &pts,
	vector<bool> &links)
{
	BezierCurve curve;
	curve._pts = pts;
	curve._links = links;
	return curve;
}

crealp wBezierWaveFromXAndCurve(
	crealp &x,
	BezierCurve &curve)
{
	return Rex::mkRex(new TrexBezierCurves0(curve._pts, curve._links, x));
}

////////////////////////////////////////////////

crealp wPhaseRex(crealp &freq)
{
	return Rex::mkRex(new TrexPhase(freq));
}
crealp wNoiseRex()
{
	return Rex::mkRex(new TrexNoise());
}
crealp wTimerRex()
{
	return Rex::mkRex(new TrexTimer());
}
crealp wEchoRex(double &delay, crealp &inp, crealp &gain)
{
	size_t delaySamples = (size_t)(delay * WGX_SAMPLESPERSECOND);
	return Rex::mkRex(new TrexEcho(inp, gain, delaySamples));
}
crealp wEchoesRex(double &delay, crealp &inp, crealp &gain)
{
	size_t delaySamples = (size_t)(delay * WGX_SAMPLESPERSECOND);
	return Rex::mkRex(new TrexEchoes(inp, gain, delaySamples));
}

crealp wDelayRex(crealp &delay, crealp &inp)
{
	return Rex::mkRex(new TrexDelay(inp, delay));
}

// crealp wConditionalRex(crealp &cond, crealp &iftrue, crealp &iffalse)
// {
// 	return Rex::mkRex(new TrexConditional(cond, iftrue, iffalse));
// }

crealp wApplyUnop(Op1 &o, crealp &b)
{
	if (g_constDoubles.contains(b))
	{
		return g_constDoubles.getOrAdd(o._f(*b));
	}
	return Rex::mkRex(new TrexFunc1(o._f, b));
}
crealp wApplyBinop(crealp &a, Op2 &o, crealp &b)
{
	if (g_constDoubles.contains(a) && g_constDoubles.contains(b))
	{
		return g_constDoubles.getOrAdd(o._g(*a, *b));
	}
	return Rex::mkRex(new TrexFunc2(o._g, a, b));
}
crealp wApplyAmbop1(OpAmbiguous &o, crealp &b)
{
	if (g_constDoubles.contains(b))
	{
		return g_constDoubles.getOrAdd(o._f(*b));
	}
	return Rex::mkRex(new TrexFunc1(o._f, b));
}
crealp wApplyAmbop2(crealp &a, OpAmbiguous &o, crealp &b)
{
	if (g_constDoubles.contains(a) && g_constDoubles.contains(b))
	{
		return g_constDoubles.getOrAdd(o._g(*a, *b));
	}
	return Rex::mkRex(new TrexFunc2(o._g, a, b));
}

////////////////////////////////////////////////

pair<string, Rex> makeParkBinding(string &name, crealp &rex)
{
	Rex r(rex, Rex::g_currentJurassicPark);
	return pair<string, Rex>(name, r);
}

shared_ptr<JurassicWorld> jurassicWorldFromParkBindings(vector<pair<string, Rex>> &bindings)
{
	shared_ptr<JurassicWorld> jw(defaultWorld->copy());
	for (size_t i = 0; i < bindings.size(); i++)
	{
		jw->addRex(bindings[i].first, bindings[i].second);
	}
	return jw;
}

////////////////////////////////////////////////

shared_ptr<JurassicWorld> jurassicWorldFromJName(string &jname)
{
	return g_jUniverse->getWorld(jname);
}

int jurassicWorldBindingSimple(string &name, shared_ptr<JurassicWorld> &world)
{
	g_jUniverse->addWorld(name, world);
	return 7;
}

shared_ptr<JurassicWorld> jworldFrom2Jworlds(
	shared_ptr<JurassicWorld> &a,
	shared_ptr<JurassicWorld> &b)
{
	shared_ptr<JurassicWorld> c(a->copy());
	c->addAllRexesFrom(b);
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
