#include "GenLang.hpp"
#include "Parsing/CharFunctions.hpp"
#include "GenlangFunctions.hpp"
#include "ActionSpecies.hpp"
#include <sstream>
using namespace std;

bool g_isStartPresent = false;

enum GenLangTkTypes
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
	GGEN,
	GGENLIST,
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

GenLang::GenLang()
	: _language(99)
//_minUnusedTkType(GMAXLANGTKTYPES)
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
	//_language.addParseRule(GGEN, {GPITCH}, {0}, gNoteFromPitch);
	//_language.addParseRule(GGEN, {GPITCH,GREAL}, {0,1}, gNoteFromPitchMultiplied);
	//_language.addParseRule(GGEN, {GREST}, {}, gRest);
	//_language.addParseRule(GGEN, {GREST,GREAL}, {1}, gRestMultiplied);
	_language.addParseRule(GGEN, {GEVENT}, {0}, gGenFromEvent);

	TkState inInt = _language.addCharStar(charIsDigit, GINT, readReal);
	TkState atDecimal = _language.addPostStarWord(".", GREAL, inInt);
	_language.addCharStar(charIsDigit, GREAL, readReal, atDecimal);
	_language.addUnaryParseRule(GREAL, GINT);

	////////////////////////////////////////////////
	// Voice name

	TkState inVoiceName = _language.addPostStarWord("@", GVOICENAME, 0);
	_language.addCharStar(charIsAlphanumeric, GVOICENAME, gVoiceIndexFromNameWithAt, inVoiceName);
	_language.addParseRule(GGEN, {GVOICENAME, GGEN}, {0, 1}, gVoiceGen);

	////////////////////////////////////////////////
	// Sequence, Parallel, Loop
	
	_language.addParseRule(GFORKELEM, {GGEN}, {0}, gEggElem);
	_language.addParseRule(GFORKELEM, {GPIPE},{}, gPipe);
	_language.addParseRule(GFORKELEM, {GPIPE2},{}, gPipe2);

	_language.addListRule(GGENLIST, GLPAREN, GGEN, GRPAREN, gGensList);
	_language.addParseRule(GGEN, {GRAND, GGENLIST}, {1}, gRandFromGens);
	_language.addParseRule(GGEN, {GCYCLE, GGENLIST}, {1}, gCycleFromGens);
	
	_language.addListRule(GGEN, GLBRACK, GFORKELEM, GRBRACK, gSeqFromGens);
	_language.addListRule(GGEN, GSMALLER, GFORKELEM, GGREATER, gParFromGens);
	_language.addParseRule(GGEN, {GLOOP, GGEN}, {1}, gLoopFromGens);
	_language.addParseRule(GGEN, {GGEN, GX, GINT}, {0, 2}, gRepeatGen1);

	////////////////////////////////////////////////
	// Operators

	_language.addParseRule(GGEN, {GGEN, GMUL, GREAL}, {0, 2}, gTimeExpandGen);
	_language.addParseRule(GGEN, {GGEN, GDIV, GREAL}, {0, 2}, gTimeContractGen);
	_language.addParseRule(GGEN, {GGEN, GUNDERSLASH, GREAL}, {0, 2}, gDurSetGen);
	_language.addParseRule(GGEN, {GGEN, GSHARP}, {0}, gSharpGen0);
	_language.addParseRule(GGEN, {GGEN, GFLAT}, {0}, gFlatGen0);
	_language.addParseRule(GGEN, {GGEN, GSHARP, GREAL}, {0, 2}, gSharpGen1);
	_language.addParseRule(GGEN, {GGEN, GFLAT, GREAL}, {0, 2}, gFlatGen1);
	_language.addParseRule(GGEN, {GGEN, GPLUS}, {0}, gOctavePlusGen);

	_language.addParseRule(GGEN, {GGEN, GINJ, GGEN}, {0,2}, gMelodyInject);

	////////////////////////////////////////////////
	// Numbers

	////////////////////////////////////////////////
	// Special operators

	// Language rules END
	////////////////////////////////////////////////
	// tkTypeReprs

	_language.setTkTypeRepr(GINT, "Int");
	_language.setTkTypeRepr(GREAL, "Real");
	_language.setTkTypeRepr(GEVENT, "Event");
	_language.setTkTypeRepr(GFORKELEM, "ForkElem");
	_language.setTkTypeRepr(GGENLIST, "GenList");
	
	////////////////////////////////////////////////
}

////////////////////////////////////////////////

map<string, size_t> GenLang::g_voiceNameToIndex;

void GenLang::initVoiceIndexes(
	map<string, size_t> &a)
{
	for (auto x : a)
	{
		g_voiceNameToIndex[x.first] = x.second;
		//cout << x.first << " -> " << x.second << endl;
	}
}

size_t GenLang::getVoiceIndex(const string &name)
{
	return g_voiceNameToIndex.at(name);
}

////////////////////////////////////////////////

Value GenLang::decodeIntoValue(const string &code)
{
	return _language.interpret(code);
}

GenLangResult GenLang::decode(const string &code)
{
	Value res = decodeIntoValue(code);
	auto el = res.get<ForkerElem>();
	el._egg->reset();
	if (g_isStartPresent)
	{
		return make_shared<EggStart>(el._egg);
	}
	else
	{
		return el._egg;
	}
	
}

////////////////////////////////////////////////
