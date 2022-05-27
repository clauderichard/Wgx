#ifndef __ISYNTHESIZER_HPP__
#define __ISYNTHESIZER_HPP__

#include "IVoice.hpp"
#include "IEffect.hpp"
#include <memory>
using namespace std;

class ISynthesizer : public IWaveSampler
{
  public:
    ////////////////////////////////////////////////
    virtual size_t addInstrument(shared_ptr<IVoice> voice) = 0;
    virtual size_t addEffect(shared_ptr<IEffect> effect) = 0;
    virtual void connectInstrumentToEffect(size_t instrIndex, size_t effectIndex) = 0;
    virtual void connectEffectToEffect(size_t inEffectIndex, size_t outEffectIndex) = 0;
    virtual void initialize() = 0;
    virtual void waveOn(size_t instrumentIndex, size_t numSamples, std::initializer_list<double> params) = 0;
    virtual void genSample() = 0;
    ////////////////////////////////////////////////

};

#endif