
#include "ActionSpecies.hpp"
#include <cstddef>
#include <memory>
#include "Egg/Event.hpp"
#include "EgglangSynthesizer.hpp"
using namespace std;

////////////////////////////////////////////////

// Do nothing (e.g. a rest instead of a note)
struct NoopEvent : public IEventAction
{
    void execute(const EventInfo &) {}
};

////////////////////////////////////////////////

struct EventNote : public IEventAction
{
    EventNote() {}
    
    void execute(const EventInfo &ei)
    {
        size_t durSamples = (size_t) (WGX_SAMPLESPERSECOND * ei._duration);
        if(!g_egglangSynthesizer)
        {
        	CRASH("!g_egglangSynthesizer")
        }
    
        g_egglangSynthesizer->waveOn(ei._voiceIndex, durSamples, {ei._pitch, ei._duration});
    }
    
};

////////////////////////////////////////////////

shared_ptr<IEventAction> g_egglangNoteAction;
shared_ptr<IEventAction> g_egglangRestAction;

void actionSpeciesInitialize()
{
	g_egglangNoteAction.reset(new EventNote());
	g_egglangRestAction.reset(new NoopEvent());
}

////////////////////////////////////////////////
