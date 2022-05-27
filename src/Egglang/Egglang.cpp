#include "Egglang.hpp"
#include "Parsing/CharFunctions.hpp"
#include "EgglangFunctions.hpp"
#include "ActionSpecies.hpp"
#include <sstream>
using namespace std;

bool g_isStartPresent = false;

enum EgglangTkTypes
{
	GINT = 1,
	GPITCH,
	GREAL,
	GEVENT,
	GEXCL,
	GLPAREN,
	GRPAREN,
	GSMALLER,
	GGREATER,
	GLBRACK,
	GRBRACK,
	GPIPE,
	GPIPE2,
	GRAND,
	GCYCLE,
	GFORKELEM,
	GVOICENAME,
	GLOOP,
	GEGG,
	GEGGLIST,
	GPLUS,
	GMINUS,
	GMUL,
	GDIV,
	GSHARP,
	GFLAT,
	GINJ,
	GX,
	GREST,
	GUNDERSLASH,
	GMAXLANGTKTYPES
};

////////////////////////////////////////////////

Egglang::Egglang()
	: _language(99)
{
	actionSpeciesInitialize();

	////////////////////////////////////////////////
	// Keywords and symbols

	_language.addWord("(", GLPAREN);
	_language.addWord(")", GRPAREN);
	_language.addWord("<", GSMALLER);
	_language.addWord(">", GGREATER);
	_language.addWord("{", GLBRACK);
	_language.addWord("}", GRBRACK);
	_language.addWord("loop", GLOOP);
	_language.addWord("'", GREST);
	_language.addWord("|", GPIPE);
	_language.addWord("||", GPIPE2);
	_language.addWord("rand", GRAND);
	_language.addWord("cycle", GCYCLE);
	_language.addWord("!", GEXCL);
	
	_language.addWord("_", GUNDERSLASH);
	_language.addWord("^", GSHARP);
	_language.addWord("v", GFLAT);
	_language.addWord("+", GPLUS);
	_language.addWord("-", GMINUS);
	_language.addWord("*", GMUL);
	_language.addWord("/", GDIV);
	_language.addWord("x", GX);
	
	_language.addWord("<<", GINJ);

	////////////////////////////////////////////////
	// Pitch, Note, Real

	_language.addChar(isPitchChar, GPITCH, gReadPitch);
	
	_language.addParseRule(GEVENT, {GREST}, {}, gRestEvent);
	_language.addParseRule(GEVENT, {GPITCH}, {0}, gEventFromPitch);
	_language.addParseRule(GEVENT, {GEVENT,GREAL}, {0,1}, gEventExpand);
	_language.addParseRule(GEVENT, {GEXCL,GEVENT}, {1}, gEventSetIsStart);
	_language.addParseRule(GEGG, {GEVENT}, {0}, gEggFromEvent);

	TkState inInt = _language.addCharStar(charIsDigit, GINT, readReal);
	TkState atDecimal = _language.addPostStarWord(".", GREAL, inInt);
	_language.addCharStar(charIsDigit, GREAL, readReal, atDecimal);
	_language.addUnaryParseRule(GREAL, GINT);

	////////////////////////////////////////////////
	// Voice name

	TkState inVoiceName = _language.addPostStarWord("@", GVOICENAME, 0);
	_language.addCharStar(charIsAlphanumeric, GVOICENAME, gVoiceIndexFromNameWithAt, inVoiceName);
	_language.addParseRule(GEGG, {GVOICENAME, GEGG}, {0, 1}, gVoiceNameGen);

	////////////////////////////////////////////////
	// Sequence, Parallel, Loop
	
	_language.addParseRule(GFORKELEM, {GEGG}, {0}, gEggElem);
	_language.addParseRule(GFORKELEM, {GPIPE},{}, gPipe);
	_language.addParseRule(GFORKELEM, {GPIPE2},{}, gPipe2);

	_language.addListRule<shared_ptr<IEgg>>(GEGGLIST, GLPAREN, GEGG, GRPAREN);
	_language.addParseRule(GEGG, {GRAND, GEGGLIST}, {1}, gRandFromEggs);
	_language.addParseRule(GEGG, {GCYCLE, GEGGLIST}, {1}, gCycleFromEggs);
	
	_language.addListRule(GEGG, GLBRACK, GFORKELEM, GRBRACK, gSeqFromEggs);
	_language.addListRule(GEGG, GSMALLER, GFORKELEM, GGREATER, gParFromEggs);
	_language.addParseRule(GEGG, {GLOOP, GEGG}, {1}, gLoopFromEgg);
	_language.addParseRule(GEGG, {GEGG, GX, GINT}, {0, 2}, gRepeatEgg1);

	////////////////////////////////////////////////
	// Operators

	_language.addParseRule(GEGG, {GEGG, GMUL, GREAL}, {0, 2}, gTimeExpandEgg);
	_language.addParseRule(GEGG, {GEGG, GDIV, GREAL}, {0, 2}, gTimeContractEgg);
	_language.addParseRule(GEGG, {GEGG, GUNDERSLASH, GREAL}, {0, 2}, gDurSetEgg);
	_language.addParseRule(GEGG, {GEGG, GSHARP}, {0}, gSharpEgg0);
	_language.addParseRule(GEGG, {GEGG, GFLAT}, {0}, gFlatEgg0);
	_language.addParseRule(GEGG, {GEGG, GSHARP, GREAL}, {0, 2}, gSharpEgg1);
	_language.addParseRule(GEGG, {GEGG, GFLAT, GREAL}, {0, 2}, gFlatEgg1);
	_language.addParseRule(GEGG, {GEGG, GPLUS}, {0}, gOctavePlusEgg);
	_language.addParseRule(GEGG, {GEGG, GMINUS}, {0}, gOctaveMinusEgg);

	_language.addParseRule(GEGG, {GEGG, GINJ, GEGG}, {0,2}, gMelodyInject);

	_language.addUnaryParseRule(GEGG, {TKTYPE_BOF,GEGG,TKTYPE_EOF}, 1);

	// Language rules END
	////////////////////////////////////////////////
	// tkTypeReprs

	_language.setTkTypeRepr(GINT, "Int");
	_language.setTkTypeRepr(GREAL, "Real");
	_language.setTkTypeRepr(GEVENT, "Event");
	_language.setTkTypeRepr(GFORKELEM, "ForkElem");
	_language.setTkTypeRepr(GEGGLIST, "EggList");
	
	////////////////////////////////////////////////
}

////////////////////////////////////////////////

map<string, size_t> Egglang::g_voiceNameToIndex;

void Egglang::initVoiceIndexes(
	map<string, size_t> &a)
{
	for (auto x : a)
	{
		g_voiceNameToIndex[x.first] = x.second;
		//cout << x.first << " -> " << x.second << endl;
	}
}

size_t Egglang::getVoiceIndex(const string &name)
{
	return g_voiceNameToIndex.at(name);
}

////////////////////////////////////////////////

Language *Egglang::getLang()
{
	return &_language;
}

Value Egglang::decodeIntoValue(const string &code)
{
	return _language.interpret(code);
}

EgglangResult Egglang::decodeLine(const string &line)
{
	Value res = _language.interpret(line);
	auto egg = res.get<EgglangResult>();
	egg->reset();
	if (g_isStartPresent)
	{
		return make_shared<EggStart>(egg);
	}
	else
	{
		return egg;
	}
}

////////////////////////////////////////////////
