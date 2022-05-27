#include "Common/All.hpp"
#include "Parsing/LanguagePair.hpp"
#include "Util/TextFileReader.hpp"
#include "Wavelang/WaveLang.hpp"
#include "Egglang/Egglang.hpp"
#include "Playback/Symphony.hpp"
#include "Playback/WavSoundStream.hpp" 
#include "Playback/WavFileWriter.hpp"

#include "Expr/Rex.hpp"
#include "Expr/RexSpecies.hpp"
#include "Expr/EquationSet.hpp"

#include "Jurassic/JurassicVoice.hpp"
#include "Jurassic/JurassicEffect.hpp"

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

bool wgxLineIsDelimiter(const string &line)
{
	return line.substr(0,4) == "----";
}
void wgxExecuteBetweenLanguages(Value &waveValue)
{
	auto wresult = waveValue.get<WaveLangResult>();
	wresult._synth->initialize();
	g_egglangSynthesizer = wresult._synth;
	Egglang::initVoiceIndexes(wresult._instrIndexes);
}

int mainPlayback()
{
	cout << "mainPlayback" << endl;

	// Interpret Wave/Egg script
	string wgxScriptName = g_mainArgs[WGX_MAINARG_WGXSCRIPTNAME];
	string wgxScriptFilePath = WGX_ROOTDIR "scripts/" + wgxScriptName + ".wgx";

	auto waveLang = make_shared<WaveLang>();
	auto egglang = make_shared<Egglang>();

	LanguagePair wgxlang(
		waveLang->getLang(), egglang->getLang(),
		wgxLineIsDelimiter,
		wgxExecuteBetweenLanguages);
	auto wgxResult = wgxlang.interpretFile(wgxScriptFilePath);

	auto synth = wgxResult.first.get<WaveLangResult>()._synth;
	auto egg = wgxResult.second.get<EgglangResult>();
	egg->reset();
	// if (g_isStartPresent)
	// {
	// 	return make_shared<EggStart>(el._egg);
	// }
	// else
	// {
	// 	return el._egg;
	// }

	// Build the main Wave Sampler
	Symphony symph(synth, egg);

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
			cout << endl << "Done." << endl;
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

	cout << endl << "Done." << endl;
	return 0;
}

class TrexWhat : public ITrex
{
  public:
	TrexWhat(crealp a)
		: _a(a) {}
	~TrexWhat() {}

	void eval()
	{
		_result = *_a / 2;
	}
	ITrex *copy()
	{
		return new TrexWhat(_a);
	}
	TrexArgList getArgs()
	{
		return TrexArgList{&_a};
	}
	bool introducesChange() const
	{
		return false;
	}
	void print(CrealpNamer &n) const
	{
		cout << "what(" << n[_a] << ")";
	}

  private:
	crealp _a;
};

class TrexFunk1 : public ITrex
{
  public:
	TrexFunk1(
		double (*f)(double),
		crealp a)
		: _f(f), _a(a) {}

	~TrexFunk1() {}

	void eval()
	{
		_result = _f(*_a);
	}

	ITrex *copy()
	{
		return new TrexFunk1(_f, _a);
	}

	TrexArgList getArgs()
	{
		return TrexArgList{&_a};
	}

	bool introducesChange() const
	{
		return false;
	}

	void print(CrealpNamer &n) const
	{
		printRealFunc(_f);
		cout << n[_a];
	}

  private:
	double (*_f)(double);
	crealp _a;
};

class TrexPhas : public ITrex
{
  public:
	TrexPhas(
		crealp freq)
		: _freq(freq) {}

	~TrexPhas() {}

	void eval()
	{
		_result = 123;
	}

	ITrex *copy()
	{
		return new TrexPhas(_freq);
	}

	TrexArgList getArgs()
	{
		return TrexArgList{&_freq};
	}

	bool introducesChange() const
	{
		return true;
	}

	void print(CrealpNamer &n) const
	{
		cout << "phaser " << n[_freq];
	}

  private:
	crealp _freq;
};

int main(int argc, char *argv[])
{
	return mainPlayback();
}