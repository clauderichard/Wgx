#ifndef __IJURASSICSYNTHELEMENT_HPP__
#define __IJURASSICSYNTHELEMENT_HPP__


#include <vector>
#include <map>
#include "Realp.hpp"
#include "RealpMap.hpp"
#include "Wave/IVoice.hpp"
#include "Util/FastArray.hpp"
#include "CrealpNamer.hpp"
using namespace std;

class ITrex;

class IJurassicSynthElement
{
  public:
	IJurassicSynthElement();
	virtual ~IJurassicSynthElement();

	void addPreTrex(ITrex *trex);
	void addTrex(ITrex *trex);
	
	void updateAllArgs(RealpMap &m);
	
	void print(CrealpNamer &namer);

  protected:
	FastArray<ITrex *> _preTrexesArray;
	FastArray<ITrex *> _trexesArray;
	
  public:
	const double *_forLeft;
	const double *_forRight;
	
	friend class JurassicWorld;

};


#endif