#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include "Genlang/IEggForker.hpp"
#include "Genlang/GenLang.hpp"
using namespace std;

bool midline = false;
GenLang genlang;

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
		if (ev._action != g_genlangRestAction)
			break;
	}
}

void eggeq(const string & x, const string & y, int numE)
{
	try
	{
		auto egg1 = genlang.decode(x);
		auto egg2 = genlang.decode(y);
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
			if (end1 || end2)
			{
				throw Wfail("egg1 end != egg2 end");
			}
			if (start1 != start2)
			{
				throw Wfail("e1 != e2 starttime");
			}
			if (e1._info._duration != e2._info._duration)
			{
				throw Wfail("e1 != e2 duration");
			}
			if (e1._info._pitch != e2._info._pitch)
			{
				throw Wfail("e1 != e2 pitch");
			}
			if (e1._action != e2._action)
			{
				throw Wfail("e1 != e2 action");
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
	//eggeq("{abc}", "{def}v5", 99);
	//eggeq("{abcde}", "{defgA}v5", 99);
	//eggeq("{ae}", "<a{'e}||>", 99);
	
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
