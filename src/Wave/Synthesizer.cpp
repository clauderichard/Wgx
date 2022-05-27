#include "Synthesizer.hpp"
#include "Common/All.hpp"
using namespace std;

////////////////////////////////////////////////

Instrument::Instrument(shared_ptr<IVoice> voice)
    : _voiceWorkers(CAPACITY_VOICES_PER_INSTRUMENT),
      _listeningEffects(CAPACITY_EFFECTS_PER_INSTRUMENT)
{
    for(size_t i=0; i<CAPACITY_VOICES_PER_INSTRUMENT; i++)
    {
        _voiceWorkers.atActualIndex(i) = voice->copy();
    }
}

Instrument::~Instrument()
{
    FASTFOR_WORKERS_CAPACITY(_voiceWorkers,p)
    {
        delete *p;
    }
}

Workforce<IVoice*>::Worker Instrument::grabVoice(size_t numSamples, std::initializer_list<double> params)
{
    auto grabbed = _voiceWorkers.grabWorker();
    grabbed.get()->resetParams(numSamples, params);
    return grabbed;
}

////////////////////////////////////////////////

PlayingVoiceInfo::PlayingVoiceInfo()
    : _effectInputWorkers(CAPACITY_EFFECTINPUTS_PER_PLAYINGVOICE) {}

////////////////////////////////////////////////

Synthesizer::Synthesizer()
    : _playingWorkers(CAPACITY_PLAYINGVOICES_PER_SYNTHESIZER),
      _effects(CAPACITY_EFFECTS_PER_SYNTHESIZER),
      _outputs(CAPACITY_OUTPUTS_PER_SYNTHESIZER)
{
}

Synthesizer::~Synthesizer() {}

size_t Synthesizer::addInstrument(shared_ptr<IVoice> voice)
{
    _instruments.emplace_back(new Instrument(voice));
    return _instruments.size() - 1;
}
size_t Synthesizer::addEffect(shared_ptr<IEffect> effect)
{
    auto &effectInfo = _effects.pushBack();
    effectInfo._effectSharedPtr = effect;
    effectInfo._effect = effect.get();
    effectInfo._outputWorker = _outputs.grabWorker();
    effectInfo._outputWorker.assign(effectInfo._effect);
    effectInfo._hasDirectOutput = true;
    return _effects._numSize - 1;
}
void Synthesizer::connectInstrumentToEffect(size_t instrIndex, size_t effectIndex)
{
    _instruments[instrIndex]->_listeningEffects.pushBack(
        _effects._array[effectIndex]._effectSharedPtr);
}
void Synthesizer::connectEffectToEffect(size_t inEffectIndex, size_t outEffectIndex)
{
    auto &inEffectInfo = _effects._array[inEffectIndex];
    auto &outEffectInfo = _effects._array[outEffectIndex];
    if (inEffectInfo._hasDirectOutput)
    {
        inEffectInfo._outputWorker.release();
        inEffectInfo._hasDirectOutput = false;
    }
    outEffectInfo._effect->addInputWave(_effects._array[inEffectIndex]._effect);
}

void Synthesizer::initialize()
{
    FASTFOR_SIZE(_effects,pe)
    {
        pe->_effect->initialize();
    }
}

void Synthesizer::waveOn(size_t instrumentIndex, size_t numSamples, std::initializer_list<double> params)
{
    auto instr = _instruments[instrumentIndex];
    auto grabbedVoice = instr->grabVoice(numSamples, params);
    // grabbed playing will be released within a loop.
    // That's why we're not keeping track of it explicitly
    auto &playing = _playingWorkers.grabWorker().get();
    playing._voiceWorker = grabbedVoice;
    playing._voice = grabbedVoice.get();
    playing._effectInputWorkers.resize(instr->_listeningEffects._numSize);
    if (instr->_listeningEffects._numSize == 0)
    {
        playing._outputWorker = _outputs.grabWorker();
        playing._outputWorker.assign(playing._voice);
    }
    else
    {
        FASTFOR_SIZES_2(instr->_listeningEffects,p1,playing._effectInputWorkers,p2)
        {
            *p2 = (*p1)->addInputWave(playing._voice);
        }
    }
}

void Synthesizer::genSample()
{
    FASTFOR_WORKERS_WORKING(_playingWorkers,p)
    {
        try
        {
            p->_voice->genSample();
        }
        catch(bool)
        {
            p->_voiceWorker.release();
            if (p->_effectInputWorkers._numSize == 0)
            {
                p->_outputWorker.release();
            }
            else
            {
                FASTFOR_SIZE(p->_effectInputWorkers,pei)
                {
                    pei->release();
                }
            }
            RELEASE_WORKER_INFASTFOR(_playingWorkers,p)
        }
    }
    FASTFOR_SIZE(_effects,pe)
    {
        pe->_effect->computeInputs();
        pe->_effect->genSample();
    }
    _left = 0;
    _right = 0;
    FASTFOR_WORKERS_WORKING(_outputs,p)
    {
        _left += (*p)->_left;
        _right += (*p)->_right;
    }
}

////////////////////////////////////////////////
