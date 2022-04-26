
#include "IEggForker.hpp"
#include "Common/Crash.hpp"
#include "ActionSpecies.hpp"
using namespace std;

////////////////////////////////////////////////

IEggForker::IEggForker()
	: _officialEndIndex(SIZE_MAX),
		_deathIndex(SIZE_MAX),
		_nextEventIndex(SIZE_MAX),
		_numPlayingThreads(0) {}
IEggForker::IEggForker(vector<shared_ptr<IEgg>> eggs)
	: _officialEndIndex(SIZE_MAX),
		_deathIndex(SIZE_MAX),
		_nextEventIndex(SIZE_MAX),
		_numPlayingThreads(0)
{
	for (auto &e : eggs)
	{
		addPart(e);
	}
}

void IEggForker::onThreadEvent(size_t threadIndex, Event &e) {}

void IEggForker::addPart(shared_ptr<IEgg> gen)
{
	EggForkerThread th;
	th._egg = gen;
	th._timeToNext = 0;
	th._isPlaying = false;
	th._isDone = false;
	_threads.push_back(th);
	_playingThreadIndexes.push_back(0);
}

void IEggForker::addOfficialEnd()
{
	_officialEndIndex = _threads.size() - 1;
}
void IEggForker::addDeath()
{
	_deathIndex = _threads.size() - 1;
}

void IEggForker::reset()
{
	killAllThreads();
	onStart();
}

Event IEggForker::pop()
{
	while (true)
	{
		updateNextIndex();
		if (_nextEventIndex == SIZE_MAX)
		{
			throw NoEventsLeft();
		}
		if (_threads[_nextEventIndex]._timeToNext > 0)
		{
			// return noop event
			Event e;
			e._action = g_genlangRestAction;
			e._info._postLag = forward();
			return e;
		}

		try
		{
			Event e = _threads[_nextEventIndex]._egg->pop();
			_threads[_nextEventIndex]._timeToNext = e._info._postLag;
			onThreadEvent(_nextEventIndex, e);
			updateNextIndex();
			if (_nextEventIndex < SIZE_MAX)
			{
				e._info._postLag = forward();
			}
			return e;
		}
		catch (OfficialEnd)
		{
			size_t i = _nextEventIndex;
			updateNextIndex();
			_threads[i]._isDone = true;
			onThreadDone(i);
		}
		catch (NoEventsLeft)
		{
			size_t i = _nextEventIndex;
			killThread(i);
			updateNextIndex();
			if (!_threads[i]._isDone)
			{
				_threads[i]._isDone = true;
				onThreadDone(i);
			}
		}
	}
}

void IEggForker::killThread(size_t index)
{
	_threads[index]._isPlaying = false;
	bool here = false;
	for(size_t i=0; i<_numPlayingThreads; i++)
	{
		if (here)
		{
			_playingThreadIndexes[i-1] = _playingThreadIndexes[i];
		}
		else if (_playingThreadIndexes[i] == index)
		{
			here = true;
		}
	}
	_numPlayingThreads --;
}

void IEggForker::killAllThreads()
{
	for (size_t i = 0; i < _threads.size(); i++)
	{
		_threads[i]._isPlaying = false;
	}
	_numPlayingThreads = 0;
}

void IEggForker::restartThread(size_t index)
{
	if (_threads[index]._isPlaying)
	{
		cout << "restarting a playing egg thread?!" << endl;
	}
	_playingThreadIndexes[_numPlayingThreads] = index;
	_numPlayingThreads ++;
	_threads[index]._egg->reset();
	_threads[index]._timeToNext = 0;
	_threads[index]._isPlaying = true;
	_threads[index]._isDone = false;
	
}

////////////////////////////////////////////////

void IEggForker::updateNextIndex()
{
	_nextEventIndex = SIZE_MAX;
	double minT;
	bool tFound = false;
	for (size_t k = 0; k < _numPlayingThreads; k++)
	{
		size_t i = _playingThreadIndexes[k];
		if (!tFound || _threads[i]._timeToNext < minT)
		{
			_nextEventIndex = i;
			minT = _threads[i]._timeToNext;
			tFound = true;
		}
	}
}

double IEggForker::forward()
{
	updateNextIndex();
	double forwardTime = _threads[_nextEventIndex]._timeToNext;
	if (forwardTime > 0)
	{
		for (size_t k = 0; k < _numPlayingThreads; k++)
		{
			size_t j = _playingThreadIndexes[k];
			if (j == _nextEventIndex)
			{
				_threads[j]._timeToNext = 0;
			}
			else
			{
				_threads[j]._timeToNext -= forwardTime;
			}
		}
	}
	return forwardTime;
}

////////////////////////////////////////////////
