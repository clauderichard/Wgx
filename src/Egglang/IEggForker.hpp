#ifndef __IEGGFORKER_HPP__
#define __IEGGFORKER_HPP__

#include "Common/Crash.hpp"
#include "Egg/Event.hpp"
#include "Egg/IEgg.hpp"
#include "ActionSpecies.hpp"
#include <vector>
#include <memory>
using namespace std;

////////////////////////////////////////////////

struct EggForkerThread
{
	shared_ptr<IEgg> _egg;
	EggTime _timeToNext;
	bool _isPlaying;
	bool _isDone;
};

class IEggForker : public IEgg
{
  public:
  
	////////////////////////////////////////////////
	// Abstract methods
	virtual void onStart() = 0;
	virtual void onThreadDone(size_t threadIndex) = 0;
	virtual void onThreadEvent(size_t threadIndex, Event &e);
	////////////////////////////////////////////////

	IEggForker();

	void addPart(shared_ptr<IEgg>);
	void addOfficialEnd();
	void addDeath();

	void reset();
	Event pop();

	void killThread(size_t index);
	void killAllThreads();
	void restartThread(size_t index);


  protected:
	vector<EggForkerThread> _threads;
	vector<size_t> _playingThreadIndexes;
	size_t _numPlayingThreads;
	size_t _officialEndIndex;
	size_t _deathIndex;
	size_t _nextEventIndex;

	void updateNextIndex();
	double forward();
};

////////////////////////////////////////////////

#endif