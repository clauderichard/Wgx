#ifndef __REX_HPP__
#define __REX_HPP__

#include "Realp.hpp"
#include <cstddef>
#include <memory>
#include <vector>
using namespace std;

class JurassicPark;
class JurassicWorld;
class ITrex;

////////////////////////////////////////////////

class Rex
{
  public:
	static void useNewPark();
	static crealp mkRex(ITrex *trex);

	Rex();
	Rex(crealp p, shared_ptr<JurassicPark> park = NULL);
	Rex(double c);

	Rex copy(); // Copies the park as well!

	vector<crealp> getVariableDependencies();
	vector<crealp> getInputDependencies();

	void print() const;

	////////////////////////////////

	crealp _val;
	shared_ptr<JurassicPark> _myPark;

	static shared_ptr<JurassicPark> g_currentJurassicPark;

	friend class JurassicWorld;
};

////////////////////////////////////////////////

#endif