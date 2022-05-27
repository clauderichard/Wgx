#include "JurassicUniverse.hpp"
#include "Wave/Synthesizer.hpp"
#include "JurassicVoice.hpp"
#include "Expr/RexProcessContext.hpp"

////////////////////////////////////////////////

void JurassicUniverse::addWorld(
	const string &name,
	shared_ptr<EquationSet> world)
{
	if (name == KEYWORD_DEFAULTWORLD_NAME)
	{
		_defaultWorld = world;
	}
	else
	{
		_worlds[name] = world;
	}
}

void JurassicUniverse::addConnection(
	const string &inname,
	const string &outname)
{
	_connections.emplace_back(inname, outname);
}

shared_ptr<EquationSet> JurassicUniverse::getWorld(const string &name)
{
	return _worlds.at(name);
}

bool JurassicUniverse::hasInputConnection(const string &outname) const
{
	for (auto conn : _connections)
	{
		if (conn.second == outname)
		{
			return true;
		}
	}
	return false;
}

void JurassicUniverse::setNameIsVoice(const string &name)
{
	_voiceNames.insert(name);
}
void JurassicUniverse::setNameIsEffect(const string &name)
{
	_effectNames.insert(name);
}

SynthesizerWithNames JurassicUniverse::toSynthesizer()
{
	if (_defaultWorld)
	{
		// add default stuff
		for (auto &w : _worlds)
		{
			w.second->addMissingFrom(*_defaultWorld);
		}
	}
	else
	{
		cout << "World named \"" << KEYWORD_DEFAULTWORLD_NAME << "\" absent!" << endl;
	}

	SynthesizerWithNames ret;
	ret._synth.reset(new Synthesizer());

	map<string, size_t> voiceIndexes;
	map<string, shared_ptr<IEffect>> effectIndexes;

	computeVoiceNames();
	computeUsedEffectNames();

	// add voices
	for (auto v : _tempVoiceNames)
	{
		cout << "Adding voice " << v << endl;
		auto world = _worlds.at(v);

		auto voice = world->toVoice();

		// auto voice = world->toVoice();
		ret._instrIndexes[v] =
			ret._synth->addInstrument(voice);
	}

	// add effects (only the used ones)
	for (auto ei = _tempUsedEffectNamesReversed.rbegin(); ei != _tempUsedEffectNamesReversed.rend(); ei++)
	{
		string e = *ei;
		cout << "Adding effect " << e << endl;
		auto world = _worlds.at(e);
		auto eff = world->toEffect();
		ret._effectIndexes[e] =
			ret._synth->addEffect(eff);
	}
	cout << endl;

	for (auto conn : _connections)
	{
		auto findOut = ret._effectIndexes.find(conn.second);
		if (findOut == ret._effectIndexes.end())
		{
			continue;
		}

		auto findInV = ret._instrIndexes.find(conn.first);
		auto findInE = ret._effectIndexes.find(conn.first);

		if (findInV != ret._instrIndexes.end())
		{
			cout << "Connecting voice " << findInV->second << " to effect " << findOut->second << endl;
			ret._synth->connectInstrumentToEffect(
				findInV->second, findOut->second);
		}
		else if (findInE != ret._effectIndexes.end())
		{
			cout << "Connecting effect " << findInE->second << " to effect " << findOut->second << endl;
			ret._synth->connectEffectToEffect(
				findInE->second, findOut->second);
		}
		else
		{
			cout << "Ignoring Connection " << conn.first << " -> " << conn.second;
			cout << " since input " << conn.first << " was ignored." << endl;
		}
	}

	return ret;
}

////////////////////////////////////////////////

void JurassicUniverse::computeVoiceNames()
{
	_tempVoiceNames.clear();
	for (auto &vn : _voiceNames)
	{
		if (_worlds.at(vn)->isValidVoice())
		{
			_tempVoiceNames.push_back(vn);
		}
		else
		{
			cout << "Voice " << vn << " is incomplete!" << endl;
			CRASH("Voice invalid")
		}
	}
	// for (auto &nw : _worlds)
	// {
	// 	if (nw.second->isValidVoice())
	// 	{
	// 		_tempVoiceNames.push_back(nw.first);
	// 	}
	// }
}

void JurassicUniverse::computeUsedEffectNames()
{
	_tempUsedEffectNamesSet.clear();
	_tempUsedEffectNamesReversed.clear();
	for (auto voiceName : _tempVoiceNames)
	{
		addUsedEffectNamesReversed(voiceName, false);
	}
}

void JurassicUniverse::addUsedEffectNamesReversed(const string &effectName, bool isEffect)
{
	if (_tempUsedEffectNamesSet.find(effectName) != _tempUsedEffectNamesSet.end())
	{
		return; // already did it
	}
	for (auto conn : _connections)
	{
		if (conn.first == effectName)
		{
			addUsedEffectNamesReversed(conn.second, true);
		}
	}
	if (isEffect)
	{
		if (_worlds.at(effectName)->isValidVoice())
		{
			cout << "Voice " << effectName << " used as an effect?!?" << endl;
			CRASH("Voice used as an effect")
		}
		_tempUsedEffectNamesReversed.push_back(effectName);
		_tempUsedEffectNamesSet.insert(effectName);
	}
}

////////////////////////////////////////////////
