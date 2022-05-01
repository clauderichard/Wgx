
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
        if(!g_genlangSynthesizer)
        {
        	CRASH("!g_genlangSynthesizer")
        }
    
        g_genlangSynthesizer->waveOn(ei._voiceIndex, durSamples, {ei._pitch, ei._duration});
    }
    
};

////////////////////////////////////////////////

shared_ptr<IEventAction> g_genlangNoteAction;
shared_ptr<IEventAction> g_genlangRestAction;

void actionSpeciesInitialize()
{
	g_genlangNoteAction.reset(new EventNote());
	g_genlangRestAction.reset(new NoopEvent());
}

////////////////////////////////////////////////
