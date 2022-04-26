#include "GenLang.hpp"
#include "Gen/IEgg.hpp"
#include "EggSpecies.hpp"
#include "EggForkerSpecies.hpp"
#include "Wave/Synthesizer.hpp"
#include "Parsing/CharFunctions.hpp"
#include <sstream>
using namespace std;

////////////////////////////////////////////////

extern bool g_isStartPresent;

void add(double &a, double &b) { a += b; }
void sub(double &a, double &b) { a -= b; }
void mul(double &a, double &b) { a *= b; }

void takesecond(double &a, double &b) { a = b; }
void takesecondSizet(size_t &a, size_t &b) { a = b; }

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
	ev._info._voiceIndex = 0;
	ev._info._pitch = pitch;
	ev._info._isStart = false;
	ev._action = g_genlangNoteAction;
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

shared_ptr<IEgg> gGenFromEvent(Event &ev)
{
	return make_shared<EggSingle>(ev);
}

Event gRestEvent()
{
	Event ev;
	ev._info._postLag = 1;
	ev._action = g_genlangRestAction;
	return ev;
}

size_t gVoiceIndexFromNameWithAt(const string &nameWithAt)
{
	string name = nameWithAt.substr(1);
	return GenLang::getVoiceIndex(name);
}

////////////////////////////////////////////////
// Seq, Par

enum ForkerElemType
{
	ForkerElemType_Egg,
	ForkerElemType_OfficialEnd,
	ForkerElemType_Death
};
struct ForkerElem
{
	ForkerElemType _type;
	shared_ptr<IEgg> _egg;
};

vector<shared_ptr<IEgg>> gGensList(vector<shared_ptr<IEgg>> &gens)
{
	return gens;
}

shared_ptr<IEgg> gSeqFromGens(vector<ForkerElem> &elems)
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

shared_ptr<IEgg> gParFromGens(vector<ForkerElem> &elems)
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

shared_ptr<IEgg> gLoopFromGens(shared_ptr<IEgg> &gen)
{
	auto ret = make_shared<EggForever>(gen);
	return ret;
}

shared_ptr<IEgg> gRandFromGens(
	vector<shared_ptr<IEgg>> &gens)
{
	auto ret = make_shared<EggRand>(gens);
	return ret;
}

shared_ptr<IEgg> gCycleFromGens(
	vector<shared_ptr<IEgg>> &gens)
{
	auto nextEggIndex = make_shared<size_t>(0);
	auto ret = make_shared<EggCycle>(gens, nextEggIndex);
	return ret;
}

shared_ptr<IEgg> gVoiceGen(
	size_t &v, shared_ptr<IEgg> &gen)
{
	return make_shared<EggMemberModify<size_t, size_t>>(
		gen, &EventInfo::refVoiceIndex, takesecondSizet, v);
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

shared_ptr<IEgg> gTimeExpandGen(
	shared_ptr<IEgg> &gen,
	double &arg)
{
	return make_shared<EggTimeFunc>(
		gen, mul, arg);
}

shared_ptr<IEgg> gTimeContractGen(
	shared_ptr<IEgg> &gen,
	double &arg)
{
	double a = 1 / arg;
	return gTimeExpandGen(gen, a);
}

shared_ptr<IEgg> gRepeatGen1(
	shared_ptr<IEgg> &gen,
	double &arg)
{
	size_t times = (size_t)(arg);
	return make_shared<EggRepeat>(
		gen, times);
}

////////////////////////////////////////////////
// Transpose Operators

shared_ptr<IEgg> gSharpGen1(
	shared_ptr<IEgg> &gen,
	double &arg)
{
	return make_shared<EggMemberModify<double, double>>(
		gen, &EventInfo::refPitch, add, arg);
}

shared_ptr<IEgg> gFlatGen1(
	shared_ptr<IEgg> &gen,
	double &arg)
{
	double a = -arg;
	return gSharpGen1(gen, a);
}

shared_ptr<IEgg> gSharpGen0(
	shared_ptr<IEgg> &gen)
{
	double a = 1;
	return gSharpGen1(gen, a);
}

shared_ptr<IEgg> gFlatGen0(
	shared_ptr<IEgg> &gen)
{
	double a = -1;
	return gSharpGen1(gen, a);
}

shared_ptr<IEgg> gOctavePlusGen(
	shared_ptr<IEgg> &gen)
{
	double a = 12;
	return gSharpGen1(gen, a);
}

shared_ptr<IEgg> gOctaveMinusGen(
	shared_ptr<IEgg> &gen)
{
	double a = -12;
	return gSharpGen1(gen, a);
}

////////////////////////////////////////////////
// Duration Operators

shared_ptr<IEgg> gDurSetGen(
	shared_ptr<IEgg> &gen,
	double &arg)
{
	return make_shared<EggMemberModify<double, double>>(
		gen, &EventInfo::refDuration, takesecond, arg);
}

////////////////////////////////////////////////
