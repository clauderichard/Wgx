#ifndef __JURASSICPARK_HPP__
#define __JURASSICPARK_HPP__

#include <string>
#include <map>
#include <set>
#include <vector>
#include <utility>
#include "Util/FastArray.hpp"
#include "Rex.hpp"
#include "RealpMap.hpp"
using namespace std;

class ITrex;
class IVoice;
class IEffect;
class IJurassicSynthElement;
class JurassicVoice;
class JurassicEffect;

////////////////////////////////////////////////

// This class's job is to have an ordered array of expressions
// to evaluate, to result in a larger expression.
// These expressions are implemented as Trex objects.
// Unlike in the movies, this JurassicPark is actually SUCCESSFUL
// at keeping all the T-rexes in line!
class JurassicPark
{
  public:
	JurassicPark();

	shared_ptr<JurassicPark> copy(RealpMap &m);

	void addTrex(ITrex *trex);

	vector<crealp> getVariableDependencies();
	vector<crealp> getInputDependencies();
	
	void print() const;

  protected:
	FastArray<shared_ptr<ITrex>> _trexesArray;

  public:
	const double *_result;

	friend class JurassicWorld;
};

////////////////////////////////////////////////

// Holds a bunch of named JurassicParks.
class JurassicWorld
{
  public:
	JurassicWorld();
	~JurassicWorld();

	shared_ptr<JurassicWorld> copy();

	void clear();
	void addRex(const string &name, Rex &rex);

	// if this and other both have the same-named rex,
	// other will overwrite this one.
	void addAllRexesFrom(shared_ptr<JurassicWorld> other);

	bool isValidVoice();
	bool isValidEffect();

	shared_ptr<IVoice> toVoice();
	shared_ptr<IEffect> toEffect();
	
	void print() const;

  private:
  
    vector<string> getRexDependenciesOrder();
	void addRexDependenciesOrder(
		vector<string> &ret,
		set<string> &done,
		const string &rexNameToAdd);

	// For every leaf dependency (e.g. $inputL or $pitch)
	bool andForAllDependencies(bool (*f)(crealp));
	
	bool hasNamedWorld(const string &name);
	
    void addAllToSynthElement(IJurassicSynthElement *ret, RealpMap &m, set<crealp> &changingValues);

	// Will resultTrex change value as the wave progresses?
	bool resultIsChanging(
		ITrex *resultTrex,
		set<crealp> &changingValues);
		
	map<string, Rex> _nameToRexMap;
	
};

////////////////////////////////////////////////

#endif