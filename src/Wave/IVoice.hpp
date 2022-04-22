#ifndef __IVOICE_HPP__
#define __IVOICE_HPP__

#include "IWaveSampler.hpp"
#include <vector>
using namespace std;

class IVoice : public IWaveSampler
{
  public:
    virtual ~IVoice(){}
    virtual IVoice *copy() = 0;
    virtual void resetParams(size_t numSamples, initializer_list<double> params) = 0;
	virtual void printt(){}
};

#endif