
#include "IEggForker.hpp"
#include "Common/Crash.hpp"
#include "ActionSpecies.hpp"
using namespace std;

////////////////////////////////////////////////

IEggForker::IEggForker()
	: _officialEndIndex(SIZE_MAX),
		_deathIndex(SIZE_MAX),
		_nextEventIndex(SIZE_MAX) {}
IEggForker::IEggForker(vector<shared_ptr<IEgg>> eggs)
	: _officialEndIndex(SIZE_MAX),
		_deathIndex(SIZE_MAX),
		_nextEventIndex(SIZE_MAX)
{
	for (auto &e : eggs)
	{
		addPart(e);
	}
}

void IEggForker::addPart(shared_ptr<IEgg> gen)
{
	EggForkerThread th;
	th._egg = gen;
	th._timeToNext = 0;
	th._isPlaying = false;
	th._isDone = false;
	_threads.push_back(th);
}

void IEggForker::addOfficialEnd()
{
	_officialEndIndex = _threads.size() - 1;
}
void IEggForker::addDeath()
{
	//if(_threads.size() > 1)
	//{
	//	size_t j = _threads.size() - 1;
	//	auto t = _threads[0];
	//	_threads[0] = _threads[j];
	//	_threads[j] = t;
	//}
	//_deathIndex = 0;
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
			_threads[i]._isPlaying = false;
			updateNextIndex();
			if (!_threads[i]._isDone)
			{
				_threads[i]._isDone = true;
				onThreadDone(i);
			}
		}
	}
}

void IEggForker::killAllThreads()
{
	for (size_t i = 0; i < _threads.size(); i++)
	{
		_threads[i]._isPlaying = false;
	}
}

void IEggForker::restartThread(size_t index)
{
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
	for (size_t i = 0; i < _threads.size(); i++)
	{
		if (_threads[i]._isPlaying &&
			(!tFound || _threads[i]._timeToNext < minT))
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
		for (size_t j = 0; j < _threads.size(); j++)
		{
			if (j == _nextEventIndex)
			{
				_threads[j]._timeToNext = 0;
			}
			else if (_threads[_nextEventIndex]._isPlaying)
			{
				_threads[j]._timeToNext -= forwardTime;
			}
		}
	}
	return forwardTime;
}

////////////////////////////////////////////////
