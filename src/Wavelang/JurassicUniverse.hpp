#ifndef __JURASSICUNIVERSE_HPP__
#define __JURASSICUNIVERSE_HPP__

#include <map>
#include <string>
#include <memory>
#include "JurassicWorld.hpp"
#include "Wave/Synthesizer.hpp"
using namespace std;

////////////////////////////////////////////////

struct SynthesizerWithNames
{
	shared_ptr<Synthesizer> _synth;
	map<string, size_t> _instrIndexes;
	map<string, size_t> _effectIndexes;
};

////////////////////////////////////////////////

class JurassicUniverse
{
  public:
	void addWorld(const string &name, shared_ptr<JurassicWorld> world);
	shared_ptr<JurassicWorld> getWorld(const string &name);
	void addConnection(const string &inname, const string &outname);
	bool hasInputConnection(const string &outname) const;

	SynthesizerWithNames toSynthesizer();

  private:
	void computeVoiceNames();
	void computeUsedEffectNames();
	void addUsedEffectNamesReversed(const string &effectName, bool isEffect);

	shared_ptr<JurassicWorld> _defaultWorld;
	map<string, shared_ptr<JurassicWorld>> _worlds;
	vector<pair<string, string>> _connections;

	vector<string> _tempVoiceNames;
	vector<string> _tempUsedEffectNamesReversed;
	set<string> _tempUsedEffectNamesSet;
};

////////////////////////////////////////////////

#endif