#include "Egglang.hpp"
#include "Egg/IEgg.hpp"
#include "EggSpecies.hpp"
#include "EggForkerSpecies.hpp"
#include "Wave/ISynthesizer.hpp"
#include "Parsing/CharFunctions.hpp"
#include "Jurassic/ValuesBank.hpp"
#include <sstream>
using namespace std;

////////////////////////////////////////////////

extern bool g_isStartPresent;

void add(double &a, double &b) { a += b; }
void sub(double &a, double &b) { a -= b; }
void mul(double &a, double &b) { a *= b; }

void takesecond(double &a, double &b) { a = b; }
void takesecondSizet(size_t &a, size_t &b) { a = b; }
void takesecondIfNegSizet(size_t &a, size_t &b)
{
	if (a == 9999)
		a = b;
}

////////////////////////////////////////////////
// Traverser pre-functions
// (call before traversing children)

////////////////////////////////////////////////
// Atoms

double g_pitchMinorScale[7] = {0, 2, 3, 5, 7, 8, 10};

bool isPitchChar(char c)
{
	return (c >= 'a' && c <= 'g') ||
		   (c >= 'A' && c <= 'G');
}

double gReadPitch(const string &s)
{
	char c = s[0];
	if (c >= 'A' && c <= 'G')
	{
		return 12 + g_pitchMinorScale[c - 'A'];
	}
	return g_pitchMinorScale[c - 'a'];
}

Event gEventFromPitch(double &pitch)
{
	Event ev;
	ev._info._postLag = 1;
	ev._info._duration = 1;
	ev._info._voiceIndex = 9999;
	ev._info._pitch = pitch;
	ev._info._isStart = false;
	ev._action = g_egglangNoteAction;
	return ev;
}

Event gEventExpand(Event &ev, double &arg)
{
	ev._info._duration *= arg;
	ev._info._postLag *= arg;
	return ev;
}

Event gEventSetIsStart(Event &ev)
{
	ev._info._isStart = true;
	g_isStartPresent = true;
	return ev;
}

Event gEventGlobVarEval(string &name, double &val)
{
	Event ev;
	ev._info._isStart = false;
	ev._info._postLag = 0;
	ev._info._duration = 0;
	ev._info._varPtr = g_valuesBank.getChanging(name);
	ev._info._assignValue = val;
	ev._action = g_egglangGlobVarEvalAction;
	return ev;
}

////////////////////////////////////////////////

struct BezierCurveG
{
	vector<StaticBezierPoint> _pts;
	vector<StaticBezierLinkType> _links;
};

StaticBezierPoint gBezierPtBuild(double &x, double &y)
{
	StaticBezierPoint pt;
	pt._x = x;
	pt._y = y;
	return pt;
}
BezierCurveG gBezierCurveBuild(
	vector<StaticBezierPoint> &pts,
	vector<StaticBezierLinkType> &links)
{
	BezierCurveG curve;
	curve._pts = pts;
	curve._links = links;
	return curve;
}

Event gEventGradientStart(string &name, BezierCurveG &curve)
{
	Event ev;
	ev._info._isStart = false;
	ev._info._postLag = 0;
	ev._info._duration = 1;
	ev._info._varPtr = g_valuesBank.getChanging(name);
	ev._info._bezierPoints = curve._pts;
	ev._info._bezierLinks = curve._links;
	ev._action = g_egglangGradientStartAction;
	return ev;
}

////////////////////////////////////////////////

shared_ptr<IEgg> gEggFromEvent(Event &ev)
{
	return make_shared<EggSingle>(ev);
}

Event gRestEvent()
{
	Event ev;
	ev._info._postLag = 1;
	ev._action = g_egglangRestAction;
	return ev;
}

size_t gVoiceIndexFromNameWithAt(const string &nameWithAt)
{
	string name = nameWithAt.substr(1);
	return Egglang::getVoiceIndex(name);
}

////////////////////////////////////////////////
// Seq, Par

shared_ptr<IEgg> gSeqFromEggs(vector<ForkerElem> &elems)
{
	auto ret = make_shared<EggSeq>();
	for (auto fe : elems)
	{
		switch (fe._type)
		{
		case ForkerElemType_Egg:
			ret->addPart(fe._egg);
			break;
		case ForkerElemType_OfficialEnd:
			ret->addOfficialEnd();
			break;
		case ForkerElemType_Death:
			ret->addDeath();
			break;
		}
	}
	return ret;
}

shared_ptr<IEgg> gParFromEggs(vector<ForkerElem> &elems)
{
	auto ret = make_shared<EggPar>();
	for (auto fe : elems)
	{
		switch (fe._type)
		{
		case ForkerElemType_Egg:
			ret->addPart(fe._egg);
			break;
		case ForkerElemType_OfficialEnd:
			ret->addOfficialEnd();
			break;
		case ForkerElemType_Death:
			ret->addDeath();
			break;
		}
	}
	return ret;
}

shared_ptr<IEgg> gLoopFromEgg(shared_ptr<IEgg> &egg)
{
	auto ret = make_shared<EggForever>(egg);
	return ret;
}

shared_ptr<IEgg> gRandFromEggs(
	vector<shared_ptr<IEgg>> &eggs)
{
	auto ret = make_shared<EggRand>(eggs);
	return ret;
}

shared_ptr<IEgg> gCycleFromEggs(
	vector<shared_ptr<IEgg>> &eggs)
{
	auto nextEggIndex = make_shared<size_t>(0);
	auto ret = make_shared<EggCycle>(eggs, nextEggIndex);
	return ret;
}

shared_ptr<IEgg> gVoiceNameGen(
	size_t &v, shared_ptr<IEgg> &egg)
{
	return make_shared<EggMemberModify<size_t, size_t>>(
		egg, &EventInfo::refVoiceIndex, takesecondIfNegSizet, v);
}

ForkerElem gEggElem(shared_ptr<IEgg> &egg)
{
	ForkerElem ret;
	ret._type = ForkerElemType_Egg;
	ret._egg = egg;
	return ret;
}
ForkerElem gPipe()
{
	ForkerElem ret;
	ret._type = ForkerElemType_OfficialEnd;
	return ret;
}
ForkerElem gPipe2()
{
	ForkerElem ret;
	ret._type = ForkerElemType_Death;
	return ret;
}

shared_ptr<IEgg> gMelodyInject(
	shared_ptr<IEgg> &left,
	shared_ptr<IEgg> &right)
{
	return make_shared<EggMelodyInject>(left, right);
}

////////////////////////////////////////////////
// Operators (foreach-event modifiers)

shared_ptr<IEgg> gTimeExpandEgg(
	shared_ptr<IEgg> &egg,
	double &arg)
{
	return make_shared<EggTimeFunc>(
		egg, mul, arg);
}

shared_ptr<IEgg> gTimeContractEgg(
	shared_ptr<IEgg> &egg,
	double &arg)
{
	double a = 1 / arg;
	return gTimeExpandEgg(egg, a);
}

shared_ptr<IEgg> gRepeatEgg1(
	shared_ptr<IEgg> &egg,
	double &arg)
{
	size_t times = (size_t)(arg);
	return make_shared<EggRepeat>(
		egg, times);
}

////////////////////////////////////////////////
// Transpose Operators

shared_ptr<IEgg> gSharpEgg1(
	shared_ptr<IEgg> &egg,
	double &arg)
{
	return make_shared<EggMemberModify<double, double>>(
		egg, &EventInfo::refPitch, add, arg);
}

shared_ptr<IEgg> gFlatEgg1(
	shared_ptr<IEgg> &egg,
	double &arg)
{
	double a = -arg;
	return gSharpEgg1(egg, a);
}

shared_ptr<IEgg> gSharpEgg0(
	shared_ptr<IEgg> &egg)
{
	double a = 1;
	return gSharpEgg1(egg, a);
}

shared_ptr<IEgg> gFlatEgg0(
	shared_ptr<IEgg> &egg)
{
	double a = -1;
	return gSharpEgg1(egg, a);
}

shared_ptr<IEgg> gOctavePlusEgg(
	shared_ptr<IEgg> &egg)
{
	double a = 12;
	return gSharpEgg1(egg, a);
}

shared_ptr<IEgg> gOctaveMinusEgg(
	shared_ptr<IEgg> &egg)
{
	double a = -12;
	return gSharpEgg1(egg, a);
}

////////////////////////////////////////////////
// Duration Operators

shared_ptr<IEgg> gDurSetEgg(
	shared_ptr<IEgg> &egg,
	double &arg)
{
	return make_shared<EggMemberModify<double, double>>(
		egg, &EventInfo::refDuration, takesecond, arg);
}

////////////////////////////////////////////////
