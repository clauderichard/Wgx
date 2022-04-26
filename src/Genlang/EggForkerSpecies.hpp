#ifndef __EGGFORKERSPECIES_HPP__
#define __EGGFORKERSPECIES_HPP__

#include "Common/Crash.hpp"
#include "Common/Capacities.hpp"
#include "IEggForker.hpp"
#include "ActionSpecies.hpp"
#include <vector>
#include <memory>
using namespace std;

// //////////////////////////////////////////////

class EggForever:public IEggForker
{
  public:
	EggForever(shared_ptr < IEgg > egg)
	:IEggForker({egg->deepcopy()})
	{
		for (size_t i = 1; i < EGG_FOREVER_THREADS; i++)
		{
			addPart(egg->deepcopy());
		}
	}

	shared_ptr < IEgg > deepcopy()
	{
		return make_shared < EggForever > (_threads[0]._egg->deepcopy());
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

// //////////////////////////////////////////////

class EggRepeat:public IEggForker
{
  public:
	EggRepeat(shared_ptr < IEgg > egg, size_t times):IEggForker(
																   {
																   egg}
	), _times(times), _timesDone(0)
	{
		for (size_t i = 1; i < EGG_REPEAT_MINTHREADS; i++)
		{
			addPart(egg->deepcopy());
		}
	}

	shared_ptr < IEgg > deepcopy()
	{
		return make_shared < EggRepeat > (_threads[0]._egg->deepcopy(), _times);
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

// //////////////////////////////////////////////

class EggPar:public IEggForker
{
  public:
	EggPar()
	{
	}

	shared_ptr < IEgg > deepcopy()
	{
		auto ret = make_shared < EggPar > ();
	  for (auto g:_threads)
		{
			if (g._egg)
			{
				ret->addPart(g._egg->deepcopy());
			}
			else
			{
				CRASH("wtf...") ret->addOfficialEnd();
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

// //////////////////////////////////////////////

class EggSeq:public IEggForker
{
  public:
	EggSeq()
	{
	}

	shared_ptr < IEgg > deepcopy()
	{
		auto ret = make_shared < EggSeq > ();
	  for (auto g:_threads)
		{
			if (g._egg)
			{
				ret->addPart(g._egg->deepcopy());
			}
			else
			{
				CRASH("wtf...") ret->addOfficialEnd();
			}
		}
		ret->_officialEndIndex = _officialEndIndex;
		ret->_deathIndex = _deathIndex;
		return ret;
	}

	void onStart()
	{
		if (_threads.size() > 0)
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

class EggMelInjectModify : public IEggEventModify
{
  public:
	EggMelInjectModify(shared_ptr<IEgg> egg)
		: IEggEventModify(egg) {}
		  
	void modifyEvent(Event &ev)
	{
		ev._info._pitch += _leftPitch;
		ev._info._postLag *= _leftDur;
		ev._info._duration *= _leftDur;
	}

	shared_ptr<IEgg> deepcopy()
	{
		return make_shared<EggMelInjectModify>(
			_egg->deepcopy());
	}

  private:
	double _leftPitch;
	double _leftDur;
	friend class EggMelodyInject;
};

class EggMelodyInject:public IEggForker
{
  public:
	EggMelodyInject(shared_ptr<IEgg> left, shared_ptr<IEgg> right)
		: _nextThreadIndex(1),
		  _left(left),
		  _right(right)
	{
		addPart(left);
		for(size_t i=0; i<EGG_MELINJECT_THREADS; i++)
		{
			addPart(NULL);
		}
	}

	shared_ptr < IEgg > deepcopy()
	{
		return make_shared < EggMelodyInject > (_left->deepcopy(), _right->deepcopy());
	}

	void onStart()
	{
		restartThread(0);
	}

	void onThreadDone(size_t threadIndex)
	{
	}
	
	void onThreadEvent(size_t threadIndex, Event &e)
	{
		if(threadIndex==0 && e._action == g_genlangNoteAction)
		{
			e._action = g_genlangRestAction;
			// Start a thread of RHS transposed & expanded
			size_t ti = _nextThreadIndex;
			if (_threads[ti]._isPlaying)
			{
				CRASH("Not enough MelInject threads");
			}
			_nextThreadIndex = (_nextThreadIndex % EGG_MELINJECT_THREADS) + 1;
			auto eggm = make_shared<EggMelInjectModify>(_right->deepcopy());
			eggm->_leftDur = e._info._duration;
			eggm->_leftPitch = e._info._pitch;
			_threads[ti]._egg = eggm;
			restartThread(ti);
		}
	}

  private:
	shared_ptr < IEgg > _left;
	shared_ptr < IEgg > _right;
	size_t _nextThreadIndex;
};

// //////////////////////////////////////////////

#endif
