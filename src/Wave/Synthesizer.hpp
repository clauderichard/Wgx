#ifndef __SYNTHESIZER_HPP__
#define __SYNTHESIZER_HPP__

#include "IVoice.hpp"
#include "IEffect.hpp"
#include "Common/Crash.hpp"
#include "Util/Workforce.hpp"
#include "Util/FastArray.hpp"
#include <vector>
using namespace std;

struct Instrument
{
  public:
    Instrument(shared_ptr<IVoice> voice);

    ~Instrument();

    Workforce<IVoice*>::Worker grabVoice(size_t numSamples, std::initializer_list<double> params);

  private:

    Workforce<IVoice*> _voiceWorkers;
  public:
    FastArray<shared_ptr<IEffect>> _listeningEffects;

};

struct PlayingVoiceInfo
{
    Workforce<IVoice*>::Worker _voiceWorker;
    Workforce<IWaveSampler *>::Worker _outputWorker;
    IVoice *_voice;
    FastArray<Workforce<IWaveSampler *>::Worker> _effectInputWorkers;
    
    PlayingVoiceInfo();
};

struct EffectInfo
{
    shared_ptr<IEffect> _effectSharedPtr;
    IEffect *_effect;
    Workforce<IWaveSampler*>::Worker _outputWorker;
    bool _hasDirectOutput;
};

class Synthesizer : public IWaveSampler
{

  public:

    Synthesizer();
    virtual ~Synthesizer(){}
      
    size_t addInstrument(shared_ptr<IVoice> voice);
    size_t addEffect(shared_ptr<IEffect> effect);
    void connectInstrumentToEffect(size_t instrIndex, size_t effectIndex);
    void connectEffectToEffect(size_t inEffectIndex, size_t outEffectIndex);

    void initialize();

    void waveOn(size_t instrumentIndex, size_t numSamples, std::initializer_list<double> params);
	  
    void genSample();

  private:
    vector<shared_ptr<Instrument>> _instruments;
    Workforce<PlayingVoiceInfo> _playingWorkers;
    FastArray<EffectInfo> _effects;
	  Workforce<IWaveSampler *> _outputs;
};

#endif