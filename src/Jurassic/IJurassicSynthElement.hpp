#ifndef __IJURASSICSYNTHELEMENT_HPP__
#define __IJURASSICSYNTHELEMENT_HPP__


#include <vector>
#include <map>
#include "Realp.hpp"
#include "RealpMap.hpp"
#include "Wave/IVoice.hpp"
#include "Util/FastArray.hpp"
#include "CrealpNamer.hpp"
#include <set>
using namespace std;

class ITrex;
struct RexProcessContext;

////////////////////////////////////////////////

enum SynthInputVarType
{
	VOICEINPUTVAR = 0,
	EFFECTINPUTVAR
};

////////////////////////////////////////////////

class IJurassicSynthElement
{
  public:
	IJurassicSynthElement();
	virtual ~IJurassicSynthElement();

	void construct(RexProcessContext &prog);

	void addPreTrex(shared_ptr<ITrex> trex);
	void addRuntimeTrex(shared_ptr<ITrex> trex);
	
	void updateAllArgs(RealpMap &m);
	
	void print(CrealpNamer &namer);

  protected:
	vector<shared_ptr<ITrex>> _allTrexes;
	FastArray<ITrex *> _preTrexesArray;
	FastArray<ITrex *> _trexesArray;
	
  public:
	const double *_forLeft;
	const double *_forRight;
	
	friend class JurassicWorld;

  protected:
    bool resultIsChanging(ITrex*, RealpMap &m);

};


#endif