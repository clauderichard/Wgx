#ifndef __JURASSICUNIVERSE_HPP__
#define __JURASSICUNIVERSE_HPP__

#include <map>
#include <string>
#include <set>
#include <memory>
#include "Expr/EquationSet.hpp"
#include "Wave/ISynthesizer.hpp"
using namespace std;

////////////////////////////////////////////////

struct SynthesizerWithNames
{
	shared_ptr<ISynthesizer> _synth;
	map<string, size_t> _instrIndexes;
	map<string, size_t> _effectIndexes;
};

////////////////////////////////////////////////

class JurassicUniverse
{
  public:
	void addWorld(const string &name, shared_ptr<EquationSet> world);
	shared_ptr<EquationSet> getWorld(const string &name);
	void addConnection(const string &inname, const string &outname);
	bool hasInputConnection(const string &outname) const;

	void setNameIsVoice(const string &name);
	void setNameIsEffect(const string &name);

	SynthesizerWithNames toSynthesizer();

  private:
	void computeVoiceNames();
	void computeUsedEffectNames();
	void addUsedEffectNamesReversed(const string &effectName, bool isEffect);

	shared_ptr<EquationSet> _defaultWorld;
	map<string, shared_ptr<EquationSet>> _worlds;
	vector<pair<string, string>> _connections;

	set<string> _voiceNames;
	set<string> _effectNames;

	vector<string> _tempVoiceNames;
	vector<string> _tempUsedEffectNamesReversed;
	set<string> _tempUsedEffectNamesSet;
};

////////////////////////////////////////////////

#endif