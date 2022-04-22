#ifndef __JURASSICVOICE_HPP__
#define __JURASSICVOICE_HPP__

#include <vector>
#include <map>
#include "Realp.hpp"
#include "Wave/IVoice.hpp"
#include "IJurassicSynthElement.hpp"
#include "Util/FastArray.hpp"
#include "ITrex.hpp"

using namespace std;

class JurassicVoice : public IVoice, public IJurassicSynthElement
{
  public:
	JurassicVoice(size_t numParams);
	virtual ~JurassicVoice();

	//void addPreTrex(ITrex *trex);
	//void addTrex(ITrex *trex);

	// This one should be super efficient,
	// because you will probably call this 44,100 times per second.
	void genSample();

	JurassicVoice *copy();
	// void optimize();

	void reset();

    const double *getParam(size_t index);
    
    void printt()
    {
		CrealpNamer n;
		n.addName(&_params[0], "param0");
		n.addName(&_params[1], "param1");
    	print(n);
    }


    void resetParams(size_t numSamples, initializer_list<double> params);

  protected:
	vector<double> _params;

	// FastArray<ITrex *> _preTrexesArray;
	// FastArray<ITrex *> _trexesArray;

	bool _numSamplesIsFinite;
	size_t _numSamplesRemaining;
	
  public:
	// const double *_forLeft;
	// const double *_forRight;

};

#endif