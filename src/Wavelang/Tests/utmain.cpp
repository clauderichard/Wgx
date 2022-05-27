#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <sstream>
#include "Wavelang/Wavelang.hpp"
using namespace std;

bool midline = false;
Wavelang wavelang;

struct Wfail
{
	string _message;
  public:
	  Wfail(const string & message):_message(message)
	{
	}
};

shared_ptr<IVoice> voiceParse(const string &x)
{
		auto voice = wavelang.decode(code);
		stringstream inpstr(noteInputs);
		
	auto waveresult = wgxlang.interpretFile(wgxScriptFilePath);
	auto synth = waveresult.get<WaveLangResult>()._synth;
	synth->
}

void voicechk(const string & code, const string &noteInputs, const string &samples)
{
	// TODO finish these UT...
	try
	{
		auto codeComplete = "V { ";
		codeComplete += code;
		codeComplete += " };"
		auto voice = voiceParse(code);
		stringstream inpstr(noteInputs);
		inpstr >> pitch;
		inpstr >> duration;

		stringstream samstr(samples);
		double expectedSample = 0;
		samstr >> expectedSample;
		double actualSample = voice.
		cout << ".";
		midline = true;
		cout.flush();
	}
	catch(Wfail & e)
	{
		if (midline)
			cout << endl;
		cout << "voicechk Failed: " << e._message << endl;
		cout << "  " << code << endl;
		cout << "  " << samples << endl;
		midline = false;
	}

}

int main(int argc, char **argv)
{
	map<string,size_t> voi;
	GenLang::initVoiceIndexes(voi);
	
	string filepath = "eggeq.txt";
	ifstream myfile(filepath);
	string line1;
	string line2;
	string line3;
	if (myfile.is_open())
	{
		while(getline(myfile, line1))
		{
			getline(myfile, line2);
			getline(myfile, line3);
			eggeq(line1,line2,64);
		}
	}
	else
	{
		throw range_error("File not found");
	}

	cout << endl << "All UTs completed." << endl;
	return 0;
}
