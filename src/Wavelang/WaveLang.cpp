#include "WaveLang.hpp"
#include "Parsing/CharFunctions.hpp"
#include "Wave/IVoice.hpp"
#include "WavelangFunctions.hpp"
#include <sstream>
using namespace std;

// WREX, WOPBIN, WOPUN, WOPAMB should each
// be following by this number of elements in the enum.
const int maxOpPriority = 4;

enum WaveLangTkTypes
{
	WINT = 1,
	WREAL,
	WNULL,
	WPITCH,
	WDURATION,
	WPARK,
	WPARKARGS,
	WPARKARGSLIST,
	WNAME,
	WVOICE,
	WEFFECT,
	WJNAME,
	WEQUAL,
	WQUESTION,
	WCOLON,
	WCOMMA,
	WPHASER,
	WECHO,
	WECHOES,
	WDELAY,
	WTIMER,
	WRANDOM,
	WONLEFT,
	WPREV,
	WOPBIN,
	WOPBIN1,
	WOPBIN2,
	WOPBIN3,
	WOPBIN4,
	WOPUN,
	WOPUN1,
	WOPUN2,
	WOPUN3,
	WOPAMB,
	WOPAMB1,
	WOPAMB2,
	WOPAMB3,
	WOPAMB4,
	WLPAREN,
	WRPAREN,
	WLBRACK,
	WRBRACK,
	// WLARG,
	// WRARG,
	WLSQUIGGLE,
	WRSQUIGGLE,
	//WLARROW,
	WRARROW,
	WLSQUIGGLE3,
	WRSQUIGGLE3,
	WSEMICOLON,
	WJWORLD,
	WNAMEDJWORLD,
	WJCONNECTION,
	WJUNIVERSECOMM,
	WJUNIVERSE,
	WREX,
	WREX1,
	WREX2,
	WREX3,
	WREX4,
	WREXATOM,
	WREXPAIR,
	WREXTRIPLE,
	WBEZIER,
	WBEZIERPT,
	WBEZIERCURVE,
	WREX2DARRAY,
	WBEZIERLINK,
	WMAXLANGTKTYPES
};

////////////////////////////////////////////////

void WaveLang::registerVariable(const string &symbol, size_t index)
{
	_language.addWordValue(symbol, _minUnusedTkType, Rex::getInputVar(0, index));
	_language.addUnaryParseRule(WREXATOM, _minUnusedTkType);
	_minUnusedTkType++;
}
void WaveLang::registerMutableVariable(const string &symbol, size_t index)
{
	_language.addWordValue(symbol, _minUnusedTkType, Rex::getInputVar(1, index));
	_language.addUnaryParseRule(WREXATOM, _minUnusedTkType);
	_minUnusedTkType++;
}

void WaveLang::registerConstant(const string &symbol, double value)
{
	_language.addWordValue(symbol, _minUnusedTkType, Rex::getConstant(value));
	_language.addUnaryParseRule(WREXATOM, _minUnusedTkType);
	_minUnusedTkType++;
}

void WaveLang::registerUnOp(const string &symbol,
							int priority,
							realfunc1 theFunc)
{
	Op1 op1;
	op1._f = theFunc;
	_language.addWordValue(symbol, _minUnusedTkType, op1);
	_language.addUnaryParseRule(WOPUN + priority, _minUnusedTkType);
	_minUnusedTkType++;
}

void WaveLang::registerBinOp(const string &symbol,
							 int priority,
							 realfunc2 theFunc)
{
	Op2 op2;
	op2._g = theFunc;
	_language.addWordValue(symbol, _minUnusedTkType, op2);
	_language.addUnaryParseRule(WOPBIN + priority, _minUnusedTkType);
	_minUnusedTkType++;
}

void WaveLang::registerAmbOp(const string &symbol,
							 int priority,
							 realfunc1 theFunc1,
							 realfunc2 theFunc2)
{
	OpAmbiguous op12;
	op12._f = theFunc1;
	op12._g = theFunc2;
	_language.addWordValue(symbol, _minUnusedTkType, op12);
	_language.addUnaryParseRule(WOPAMB + priority, _minUnusedTkType);
	_minUnusedTkType++;
}

////////////////////////////////////////////////

WaveLang::WaveLang()
	: _language(99),
	  _minUnusedTkType(WMAXLANGTKTYPES)
{
	////////////////////////////////////////////////
	// Keywords and symbols

	_language.addWord("(", WLPAREN);
	_language.addWord(")", WRPAREN);
	_language.addWord("[", WLBRACK);
	_language.addWord("]", WRBRACK);
	// _language.addWord("<", WLARG);
	// _language.addWord(">", WRARG);
	_language.addWord("{", WLSQUIGGLE);
	_language.addWord("}", WRSQUIGGLE);
	_language.addWord("voice", WVOICE);
	_language.addWord("effect", WEFFECT);
	//_language.addWord("<-", WLARROW);
	_language.addWord("->", WRARROW);
	_language.addWord("{{{", WLSQUIGGLE3);
	_language.addWord("}}}", WRSQUIGGLE3);
	_language.addWord(";", WSEMICOLON);
	_language.addWord("=", WEQUAL);
	_language.addWord("?", WQUESTION);
	_language.addWord(":", WCOLON);
	_language.addWord(",", WCOMMA);
	_language.addWord("random", WRANDOM);
	_language.addWord("onleft", WONLEFT);
	_language.addWord("prev", WPREV);
	_language.addWord("timer", WTIMER);
	_language.addWord("phaser", WPHASER);
	_language.addWord("echo", WECHO);
	_language.addWord("echoes", WECHOES);
	_language.addWord("delay", WDELAY);
	_language.addWord("bezier", WBEZIER);
	_language.addWordValue("--", WBEZIERLINK, BezierLinkType::LINEAR);
	_language.addWordValue("~-", WBEZIERLINK, BezierLinkType::QUADRATICFLATLEFT);
	_language.addWordValue("-~", WBEZIERLINK, BezierLinkType::QUADRATICFLATRIGHT);
	_language.addWordValue("~~", WBEZIERLINK, BezierLinkType::CUBICFLAT);

	////////////////////////////////////////////////
	// Constants
	double pi = 3.14159265358979323975;
	// registerConstant("pi", pi);
	// registerConstant("tau", 2 * pi);
	// registerConstant("baseFreq", 440);
	// registerConstant("semitoneFreqFactor", std::pow(2.0, 1.0 / 12));
	_language.addWordValue<crealp>("null", WNULL, NULL);
	_language.addUnaryParseRule(WREXATOM, WNULL);

	////////////////////////////////////////////////
	// Variables

	registerVariable(KEYWORD_INPUT_PITCH, 0);
	registerVariable(KEYWORD_INPUT_DURATION, 1);
	registerMutableVariable(KEYWORD_INPUT_AUDIOLEFT, 0);
	registerMutableVariable(KEYWORD_INPUT_AUDIORIGHT, 1);

	////////////////////////////////////////////////
	// Operators

	registerUnOp("sin", 4, std::sin);
	registerUnOp("cos", 4, std::cos);
	registerUnOp("exp", 4, std::exp);
	registerUnOp("log", 4, std::log);

	registerBinOp("+", 2, realfunc2_add);
	registerAmbOp("-", 2, realfunc1_neg, realfunc2_sub);
	registerBinOp("*", 3, realfunc2_mul);
	registerBinOp("/", 3, realfunc2_div);
	registerBinOp("**", 4, realfunc2_pow);

	// registerBinOp(">", 1, realfunc2_gt);
	// registerBinOp("<", 1, realfunc2_lt);

	////////////////////////////////////////////////
	// Numbers

	TkState inInt = _language.addCharStar(charIsDigit, WINT, readReal);
	TkState atDecimal = _language.addPostStarWord(".", WREAL, inInt);
	_language.addCharStar(charIsDigit, WREAL, readReal, atDecimal);

	// Names. Make sure this occurs AFTER the keywords!
	TkState inName = _language.addCharStar(charIsAlphaLower, WNAME);
	_language.addCharStar(charIsAlphanumeric, WNAME, inName);
	TkState inJName = _language.addCharStar(charIsAlphaUpper, WJNAME);
	_language.addCharStar(charIsAlphanumeric, WJNAME, inJName);
	//	_language.addCharStar(charIsAlphaLower, WNAME);
	//	_language.addCharStar(charIsAlphanumeric, WJNAME, WJNAME);

	_language.addParseRule(WREXATOM, {WNAME}, {0}, rexVarFromName);
	_language.addUnaryParseRule(WREAL, WINT);
	_language.addParseRule(WREXATOM, {WREAL}, {0}, rexFromReal);

	////////////////////////////////////////////////
	// Operator rules (generic)

	for (int degree = 0; degree <= maxOpPriority; degree++)
	{
		TkType r = degree + WREX;
		TkType o1 = degree + WOPUN;
		TkType o2 = degree + WOPBIN;
		TkType o12 = degree + WOPAMB;
		// apply op1
		for (int degree2 = degree + 1; degree2 <= maxOpPriority + 1; degree2++)
		{
			TkType r2 = degree2 + WREX;
			_language.addParseRule(r, {o1, r2}, {0, 1}, wApplyUnopRex);
			_language.addParseRule(r, {o12, r2}, {0, 1}, wApplyAmbop1Rex);
		}
		// apply op2
		for (int degree2 = degree; degree2 <= maxOpPriority + 1; degree2++)
		{
			TkType r2 = degree2 + WREX;
			for (int degree3 = degree + 1; degree3 <= maxOpPriority + 1; degree3++)
			{
				TkType r3 = degree3 + WREX;
				_language.addParseRule(r, {r2, o2, r3}, {0, 1, 2}, wApplyBinopRex);
				_language.addParseRule(r, {r2, o12, r3}, {0, 1, 2}, wApplyAmbop2Rex);
			}
		}
	}
	// upgrade to wrex
	for (int degree = 1; degree <= maxOpPriority + 1; degree++)
	{
		_language.addUnaryParseRule(WREX, degree + WREX);
	}
	_language.addUnaryParseRule(WREXATOM, {WLPAREN, WREX, WRPAREN}, 1);

	////////////////////////////////////////////////
	// Special operators

	_language.addParseRule(WREXATOM, {WRANDOM,WLPAREN,WREX,WCOMMA,WREX,WRPAREN}, {2,4}, wRandomRex);
	_language.addParseRule(WREXATOM, {WONLEFT,WLPAREN,WREX,WCOMMA,WREX,WRPAREN}, {2,4}, wOnLeftRex);
	_language.addParseRule(WREXATOM, {WPREV,WREXATOM}, {1}, wPrevRex);
	_language.addParseRule(WREXATOM, {WTIMER}, {}, wTimerRex);
	_language.addParseRule(WREXATOM, {WPHASER, WREXATOM}, {1}, wPhaseRex);
	// _language.addParseRule(WREXATOM, {WECHO, WREAL, WREXATOM, WCOMMA, WREXATOM}, {1, 2, 4}, wEchoRex);
	// _language.addParseRule(WREXATOM, {WECHOES, WREAL, WREXATOM, WCOMMA, WREXATOM}, {1, 2, 4}, wEchoesRex);
	_language.addParseRule(WREXATOM, {WDELAY, WREXATOM, WCOMMA, WREXATOM}, {1, 3}, wDelayRex);
	// _language.addParseRule(WREX, {WREXATOM, WQUESTION, WREXATOM, WCOLON, WREXATOM}, {0, 2, 4}, wConditionalRex);

	////////////////////////////////////////////////
	// Bezier curve

	_language.addParseRule(WBEZIERPT, {WREX, WCOMMA, WREX}, {0, 2}, wBezierPtExBuild);

	_language.addListRule2(WBEZIERCURVE, WLBRACK, WBEZIERPT, WBEZIERLINK, WRBRACK, wBezierCurveExBuild);
	_language.addParseRule(WREXATOM, {WREXATOM, WBEZIERCURVE}, {0, 1}, wBezierWaveExBuild);

	////////////////////////////////////////////////
	// Build JurassicParks

	// Simple
	auto parkRule = _language.addParseRule(WPARK, {WNAME, WEQUAL, WREX, WSEMICOLON}, {0, 2}, wEquation);
	
	// Function bind
	auto parkBindRuleWithArgs = _language.addParseRule(
		WPARK, {WPARKARGS, WEQUAL, WREX, WSEMICOLON}, {0, 2}, makeCustomFunctionDefinition);
	
	// function declaration / call
	_language.addListRuleDelim<Rex>(WPARKARGSLIST, WLPAREN, WREX, WCOMMA, WRPAREN);
	_language.addParseRule(WPARKARGS, {WNAME, WPARKARGSLIST}, {0,1}, wCustomFunctionArgs);
	
	// RHS function call
	_language.addParseRule(WREXATOM, {WPARKARGS}, {0}, makeCustomFunctionCallRex);
	
	////////////////////////////////////////////////
	// Build EquationSets

	_language.addListRule(WJWORLD, WLSQUIGGLE, WPARK, WRSQUIGGLE, equationSetFromParkBindings);

	_language.addParseRule(WJWORLD, {WJWORLD, WJWORLD}, {0, 1}, jworldFrom2Jworlds);
	_language.addParseRule(WJWORLD, {WJNAME}, {0}, jurassicWorldFromJName);

	_language.addParseRule(WJUNIVERSECOMM, {WJNAME, WEQUAL, WJWORLD, WSEMICOLON}, {0, 2}, jurassicWorldBindingSimple);
	_language.addParseRule(WJUNIVERSECOMM, {WVOICE, WJNAME, WEQUAL, WJWORLD, WSEMICOLON}, {1, 3}, jurassicWorldBindingVoice);
	_language.addParseRule(WJUNIVERSECOMM, {WEFFECT, WJNAME, WEQUAL, WJWORLD, WSEMICOLON}, {1, 3}, jurassicWorldBindingEffect);
	_language.addParseRule(WJUNIVERSECOMM, {WJNAME, WRARROW, WJNAME, WSEMICOLON}, {0, 2}, connectionFromNames);

	auto universeRule = _language.addListRule(WJUNIVERSE, TKTYPE_BOF, WJUNIVERSECOMM, TKTYPE_EOF, wUniverseFromNamedWorlds);
	_language.addNodePreFunc(universeRule, newJUniverse);

	// Language rules END
	////////////////////////////////////////////////
	// tkTypeReprs

	_language.setTkTypeRepr(WNAME, "Name");
	_language.setTkTypeRepr(WJWORLD, "JWorld");
	_language.setTkTypeRepr(WJNAME, "JName");
	_language.setTkTypeRepr(WPARK, "Park");
	_language.setTkTypeRepr(WPARKARGSLIST, "ParkArgsList");
	_language.setTkTypeRepr(WPARKARGS, "ParkArgs");
	_language.setTkTypeRepr(WREXTRIPLE, "RexTriple");
	_language.setTkTypeRepr(WBEZIERPT, "BezierPt");
	_language.setTkTypeRepr(WBEZIERCURVE, "BezierCurve");
	_language.setTkTypeRepr(WBEZIERLINK, "BezLink");
	_language.setTkTypeRepr(WREXPAIR, "RexPair");
	_language.setTkTypeRepr(WREXATOM, "RexAtom");
	_language.setTkTypeRepr(WREX, "Rex");
	_language.setTkTypeRepr(WOPUN, "OpUn");
	_language.setTkTypeRepr(WOPBIN, "OpBin");
	_language.setTkTypeRepr(WOPAMB, "OpAmb");
	for (int degree = 1; degree <= maxOpPriority; degree++)
	{
		stringstream ss1;
		ss1 << "Rex" << degree;
		_language.setTkTypeRepr(WREX + degree, ss1.str());
		stringstream ss2;
		ss2 << "OpUn" << degree;
		_language.setTkTypeRepr(WOPUN + degree, ss2.str());
		stringstream ss3;
		ss3 << "OpBin" << degree;
		_language.setTkTypeRepr(WOPBIN + degree, ss3.str());
		stringstream ss4;
		ss4 << "OpAmb" << degree;
		_language.setTkTypeRepr(WOPAMB + degree, ss4.str());
	}

}

////////////////////////////////////////////////

Language *WaveLang::getLang()
{
	return &_language;
}

Value WaveLang::decodeIntoValue(const string &code)
{
	return _language.interpret(code);
}

// WaveLangResult WaveLang::decodeFile(const string &filename)
// {
// 	Value res = _language.interpretFile(filename);
// 	return res.get<WaveLangResult>();
// }

////////////////////////////////////////////////
