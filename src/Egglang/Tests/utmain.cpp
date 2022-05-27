#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <sstream>
#include "Egglang/IEggForker.hpp"
#include "Egglang/Egglang.hpp"
using namespace std;

bool midline = false;
Egglang egglang;

struct Wfail
{
	string _message;
  public:
	  Wfail(const string & message):_message(message)
	{
	}
};

void nextEv(shared_ptr<IEgg> &egg, Event & ev, double &start)
{
	while (true)
	{
		start += ev._info._postLag;
		ev = egg->pop();
		if (ev._action != g_egglangRestAction)
			break;
	}
}

void eggeq(const string & x, const string & y, int numE)
{
	try
	{
		auto egg1 = egglang.decode(x);
		auto egg2 = egglang.decode(y);
		Event e1;
		e1._info._postLag=0;
		Event e2;
		e2._info._postLag=0;
		double start1 = 0;
		double start2 = 0;
		bool end1 = false;
		bool end2 = false;
		for (int i = 0; i < numE; i++)
		{
			stringstream ss;
			ss << i;
			string istr = ss.str();
			try
			{
				nextEv(egg1, e1, start1);
			}
			catch(OfficialEnd)
			{
				end1 = true;
			}
			catch(NoEventsLeft)
			{
				end1 = true;
			}
			try
			{
				nextEv(egg2, e2, start2);
			}
			catch(OfficialEnd)
			{
				end2 = true;
			}
			catch(NoEventsLeft)
			{
				end2 = true;
			}
			if (end1 && end2)
				break;
			if (end1)
			{
				throw Wfail(istr + "egg1 ended before egg2");
			}
			else if (end2)
			{
				throw Wfail(istr + "egg2 ended before egg1");
			}
			if (start1 != start2)
			{
				throw Wfail(istr + "e1 != e2 starttime");
			}
			if (e1._info._duration != e2._info._duration)
			{
				throw Wfail(istr + "e1 != e2 duration");
			}
			if (e1._info._pitch != e2._info._pitch)
			{
				throw Wfail(istr + "e1 != e2 pitch");
			}
			if (e1._info._voiceIndex != e2._info._voiceIndex)
			{
				throw Wfail(istr + "e1 != e2 voiceIndex");
			}
			if (e1._action != e2._action)
			{
				throw Wfail(istr + "e1 != e2 action");
			}

		}
		cout << ".";
		midline = true;
		cout.flush();
	}
	catch(Wfail & e)
	{
		if (midline)
			cout << endl;
		cout << "eggeq Failed: " << e._message << endl;
		cout << "  " << x << endl;
		cout << "  " << y << endl;
		midline = false;
	}

}

int main(int argc, char **argv)
{
	map<string,size_t> voi;
	voi["a"] = 0;
	voi["b1"] = 1;
	voi["c2c"] = 2;
	voi["dD"] = 3;
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
