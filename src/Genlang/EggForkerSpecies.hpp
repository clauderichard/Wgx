#ifndef __EGGFORKERSPECIES_HPP__
#define __EGGFORKERSPECIES_HPP__

#include "Common/Crash.hpp"
#include "Common/Capacities.hpp"
#include "IEggForker.hpp"
#include "ActionSpecies.hpp"
#include <vector>
#include <memory>
using namespace std;

////////////////////////////////////////////////

class EggForever : public IEggForker
{
  public:
	EggForever(shared_ptr<IEgg> egg)
		: IEggForker({egg->deepcopy()})
	{
		for (size_t i = 1; i < EGG_FOREVER_THREADS; i++)
		{
			addPart(egg->deepcopy());
		}
	}

	shared_ptr<IEgg> deepcopy()
	{
		return make_shared<EggForever>(_threads[0]._egg->deepcopy());
	}

	void onStart()
	{
		restartThread(0);
	}

	void onThreadDone(size_t index)
	{
		size_t i = (index + 1) % EGG_FOREVER_THREADS;
		if (_threads[i]._isPlaying)
		{
			CRASH("looping not parallel enough!");
		}
		restartThread(i);
	}
};

////////////////////////////////////////////////

class EggRepeat : public IEggForker
{
  public:
	EggRepeat(shared_ptr<IEgg> egg, size_t times)
		: IEggForker({egg}),
		  _times(times),
		  _timesDone(0)
	{
		for (size_t i = 1; i < EGG_REPEAT_MINTHREADS; i++)
		{
			addPart(egg->deepcopy());
		}
	}

	shared_ptr<IEgg> deepcopy()
	{
		return make_shared<EggRepeat>(_threads[0]._egg->deepcopy(), _times);
	}

	void onStart()
	{
		_timesDone = 0;
		restartThread(0);
	}

	void onThreadDone(size_t index)
	{
		_timesDone++;
		if (_timesDone < _times)
		{
			size_t i = (index + 1) % EGG_REPEAT_MINTHREADS;
			if (_threads[i]._isPlaying)
			{
				CRASH("loopingRepeat not parallel enough!");
			}
			restartThread(i);
		}
		else
		{
			throw OfficialEnd();
		}
	}

  private:
	size_t _times;
	size_t _timesDone;
};

////////////////////////////////////////////////

class EggPar : public IEggForker
{
  public:
	EggPar() {}

	shared_ptr<IEgg> deepcopy()
	{
		auto ret = make_shared<EggPar>();
		for (auto g : _threads)
		{
			if (g._egg)
			{
				ret->addPart(g._egg->deepcopy());
			}
			else
			{
				CRASH("wtf...")
				ret->addOfficialEnd();
			}
		}
		ret->_officialEndIndex = _officialEndIndex;
		ret->_deathIndex = _deathIndex;
		return ret;
	}

	void onStart()
	{
		for (size_t i = 0; i < _threads.size(); i++)
		{
			restartThread(i);
		}
	}

	void onThreadDone(size_t threadIndex)
	{
		if (threadIndex == _deathIndex)
		{
			killAllThreads();
			throw NoEventsLeft();
		}
		else if (threadIndex == _officialEndIndex ||
				 (_officialEndIndex == SIZE_MAX && threadIndex == _threads.size() - 1))
		{
			throw OfficialEnd();
		}
	}
};

////////////////////////////////////////////////

class EggSeq : public IEggForker
{
  public:
	EggSeq() {}

	shared_ptr<IEgg> deepcopy()
	{
		auto ret = make_shared<EggSeq>();
		for (auto g : _threads)
		{
			if (g._egg)
			{
				ret->addPart(g._egg->deepcopy());
			}
			else
			{
				CRASH("wtf...")
				ret->addOfficialEnd();
			}
		}
		ret->_officialEndIndex = _officialEndIndex;
		ret->_deathIndex = _deathIndex;
		return ret;
	}

	void onStart()
	{
		restartThread(0);
	}

	void onThreadDone(size_t threadIndex)
	{
		if (threadIndex == _deathIndex)
		{
			killAllThreads();
			throw NoEventsLeft();
		}
		size_t i = threadIndex + 1;
		if (i < _threads.size())
		{
			restartThread(i);
		}
		if (threadIndex == _officialEndIndex ||
			(_officialEndIndex == SIZE_MAX && threadIndex == _threads.size() - 1))
		{
			throw OfficialEnd();
		}
	}
};

////////////////////////////////////////////////

class EggMelodyInject : public IEggForker
{
  public:
	EggMelodyInject(shared_ptr<IEgg> left, shared_ptr<IEgg> right) {}

	shared_ptr<IEgg> deepcopy()
	{
		return make_shared<EggMelodyInject>(_left->deepcopy(), _right->deepcopy());
	}

	void onStart()
	{
		NOTIMPL("EggMelodyInject::onStart")
	}

	void onThreadDone(size_t threadIndex)
	{
		NOTIMPL("EggMelodyInject::onThreadDone")
	}

  private:
	shared_ptr<IEgg> _left;
	shared_ptr<IEgg> _right;

};

////////////////////////////////////////////////

#endif