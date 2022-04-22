#include "Common/All.hpp"
#include "Util/TextFileReader.hpp"
//#include "Gen/IEgg.hpp"
//#include "Genlang/EventSpecies.hpp"
//#include "Wavelang/ValuesSets.hpp"
#include "Wavelang/WaveLang.hpp"
//#include "Wave/Synthesizer.hpp"
#include "Genlang/GenLang.hpp"
//#include "Wavelang/JurassicWorld.hpp"
//#include "Wavelang/JurassicUniverse.hpp"
//#include "Wavelang/Rex.hpp"
#include "Playback/Symphony.hpp"
#include "Playback/WavSoundStream.hpp" 
#include "Playback/WavFileWriter.hpp"
//#include "Wave/IVoice.hpp"
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

////////////////////////////////////////////////

class MainArgs
{
  public:
	MainArgs()
	{
		string wgxRootDir = WGX_ROOTDIR;
		readMainArgsFile(wgxRootDir + "/scripts/mainargs.txt");
	}

	string operator[](const string &name)
	{
		return _mainArgs.at(name);
	}
	bool getBoolean(const string &name)
	{
		string v = _mainArgs.at(name);
		return stoi(v.c_str());
	}
	double getDouble(const string &name)
	{
		string v = _mainArgs.at(name);
		return atof(v.c_str());
	}
	int getInt(const string &name)
	{
		string v = _mainArgs.at(name);
		return atoi(v.c_str());
	}
	size_t getSizet(const string &name)
	{
		string v = _mainArgs.at(name);
		return atoi(v.c_str());
	}

  private:
	void readMainArgsFile(
		const string &filePath)
	{
		ifstream myfile(filePath);
		if (!myfile.is_open())
		{
			throw out_of_range("File " + filePath + " not found!");
		}
		// get space-separated things including '=' signs
		vector<string> pieces;
		string s;
		while (true)
		{
			if (myfile.eof())
			{
				break;
			}
			myfile >> s;
			pieces.push_back(s);
		}
		// For each '=' add the things around it
		for (size_t i = 0; i < pieces.size(); i++)
		{
			if (pieces[i] == "=")
			{
				string name = pieces[i - 1];
				string value = pieces[i + 1];
				_mainArgs[name] = value;
			}
		}
	}
	map<string, string> _mainArgs;
};

MainArgs g_mainArgs;

////////////////////////////////////////////////

//void addNote(shared_ptr<Synthesizer> synth, shared_ptr<FlatSeqGenerator> gen, size_t w, double dur, double pitch)
//{
//	shared_ptr<EventNote> n(new EventNote(synth));
//	n->duration = dur;
//	n->pitch = pitch;
//	n->waveIndex = w;
//	gen->addEvent(n, dur);
//}

shared_ptr<IEgg> getTheGenerator(shared_ptr<Synthesizer> synth)
{
	GenLang genLang;
	g_genlangSynthesizer = synth;
	auto gresult = genLang.decode("loop [ae (cg) (dA)]");

	return gresult;
}

////////////////////////////////////////////////

int mainPlaybackTry2()
{
	cout << "mainPlaybackTry2" << endl;

	// Interpret Wave script
	string waveScriptName = g_mainArgs[WGX_MAINARG_WAVESCRIPTNAME];
	string waveFilePath = WGX_ROOTDIR "scripts/" + waveScriptName + ".wx";
	//string waveCode = readFile(waveFilePath);

	string genScriptName = g_mainArgs[WGX_MAINARG_GENSCRIPTNAME];
	string genFilePath = WGX_ROOTDIR "scripts/" + genScriptName + ".gx";
	string genCode = readFile(genFilePath);

	WaveLang waveLang;
	auto wresult = waveLang.decode(waveFilePath);

	// Build Synthesizer
	shared_ptr<Synthesizer> synth = wresult._synth;
	synth->initialize();

	//return 0;

	// Build Generator
	GenLang genLang;
	g_genlangSynthesizer = synth;
	GenLang::initVoiceIndexes(wresult._instrIndexes);
	auto gen = genLang.decode(genCode);
	//auto gen = getTheGenerator(synth);

	// Build the main Wave Sampler
	Symphony symph(synth, gen);

#ifdef SFML_SUPPORTED
	bool playDirect = g_mainArgs.getBoolean(WGX_MAINARG_PLAYDIRECT);
	if (playDirect)
	{
		try
		{
			WavSoundStream music(symph, WGX_SAMPLESPERSECOND, -1);
			cout << "SoundStream created." << endl;

			music.play();
			cout << "  Playing " << endl;

			while (true)
			{
				if (music.getStatus() == sf::Sound::Stopped)
					break;
			}
		}
		catch (const std::exception &e)
		{
			cout << "streamSong ERROR:" << endl;
			cout << " " << e.what() << endl;
			return 1;
		}
	}
#endif

	//cout << "SFML not supported so no playback." << endl;
	string outputWavFileName = string(WGX_WAVOUTPUTDIR) + g_mainArgs[WGX_MAINARG_OUTFILENAME] + ".wav";
	double numSeconds = g_mainArgs.getDouble(WGX_MAINARG_SECONDS);
	size_t numSamples = size_t(WGX_SAMPLESPERSECOND * numSeconds);
	writeWavFile(symph, WGX_SAMPLESPERSECOND, numSamples, outputWavFileName);

	return 0;
}

int main(int argc, char *argv[])
{
	return mainPlaybackTry2();
}