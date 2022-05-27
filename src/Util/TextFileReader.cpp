#include "Common/Crash.hpp"
#include "TextFileReader.hpp"
#include <fstream>
#include <sstream>
using namespace std;

// Comment line = one that starts with #
// (maybe preceded by whitespace)
bool isCommentLine(const string &line)
{
	for (auto c : line)
	{
		if (c == '#')
		{
			return true;
		}
		if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
		{
			return false;
		}
	}
	return false;
}

string readFile(const string &filepath)
{
	ifstream myfile(filepath);
	stringstream sstr;
	string line;
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			sstr << '\n';
			if (!isCommentLine(line))
			{
				sstr << line;
			}
		}
	}
	else
	{
		throw range_error("File not found");
	}

	return sstr.str();
}
