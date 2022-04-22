#ifndef __EGGSPECIES_HPP__
#define __EGGSPECIES_HPP__

#include "Common/Crash.hpp"
#include "Common/Capacities.hpp"
#include "Gen/Event.hpp"
#include "Gen/IEgg.hpp"
#include "IEggForker.hpp"
#include "ActionSpecies.hpp"
#include <vector>
#include <memory>
using namespace std;

////////////////////////////////////////////////

class EggSingle : public IEgg
{
  public:
	EggSingle(Event e)
		: _event(e),
		  _popped(false) {}

	shared_ptr<IEgg> deepcopy()
	{
		return make_shared<EggSingle>(_event);
	}

	void reset()
	{
		_popped = false;
	}

	Event pop()
	{
		if (_popped)
		{
			throw NoEventsLeft();
		}
		_popped = true;
		return _event;
	}

  private:
	Event _event;
	bool _popped;
};

////////////////////////////////////////////////

class IEggSelector : public IEgg
{
  public:
	virtual size_t getNextIndex() = 0;

	IEggSelector(vector<shared_ptr<IEgg>> &eggs)
		: _eggs(eggs)
	{
	}

	void reset()
	{
		_egg = _eggs[getNextIndex()];
		_egg->reset();
	}

	Event pop()
	{
		return _egg->pop();
	}

	static vector<shared_ptr<IEgg>> deepcopies(vector<shared_ptr<IEgg>> &eggs)
	{
		vector<shared_ptr<IEgg>> eggCopies;
		for (auto &e : eggs)
		{
			eggCopies.push_back(e->deepcopy());
		}
		return eggCopies;
	}

  protected:
	vector<shared_ptr<IEgg>> _eggs;

  private:
	shared_ptr<IEgg> _egg;
};

////////////////////////////////////////////////

class EggRand : public IEggSelector
{
  public:
	EggRand(vector<shared_ptr<IEgg>> &eggs)
		: IEggSelector(eggs)
	{
	}

	shared_ptr<IEgg> deepcopy()
	{
		auto cops = deepcopies(_eggs);
		return make_shared<EggRand>(cops);
	}

	size_t getNextIndex()
	{
		return rand() % _eggs.size();
	}

};

////////////////////////////////////////////////

class EggCycle : public IEggSelector
{
  public:
	EggCycle(vector<shared_ptr<IEgg>> &eggs, shared_ptr<size_t> nextEggIndex)
		: IEggSelector(eggs),
		  _nextEggIndex(nextEggIndex)
	{
	}

	shared_ptr<IEgg> deepcopy()
	{
		auto cops = deepcopies(_eggs);
		// use same nextEggIndex, no deepcopy of that
		return make_shared<EggCycle>(cops, _nextEggIndex);
	}

	size_t getNextIndex()
	{
		size_t ret = *_nextEggIndex;
		*_nextEggIndex = (*_nextEggIndex + 1) % _eggs.size();
		return ret;
	}

  private:
	shared_ptr<size_t> _nextEggIndex;
};

////////////////////////////////////////////////

class IEggEventModify : public IEgg
{
  public:
	IEggEventModify(shared_ptr<IEgg> egg)
		: _egg(egg) {}

	virtual void modifyEvent(Event &) = 0;

	void reset()
	{
		_egg->reset();
	}

	Event pop()
	{
		Event ev = _egg->pop();
		modifyEvent(ev);
		return ev;
	}

  protected:
	shared_ptr<IEgg> _egg;
};

////////////////////////////////////////////////

class EggTimeFunc : public IEggEventModify
{
  public:
	EggTimeFunc(shared_ptr<IEgg> egg,
				void (*fun)(double &, double &),
				double arg)
		: IEggEventModify(egg),
		  _fun(fun),
		  _arg(arg) {}
		  
	void modifyEvent(Event &ev)
	{
		_fun(ev._info._postLag, _arg);
		_fun(ev._info._duration, _arg);
	}

	shared_ptr<IEgg> deepcopy()
	{
		return make_shared<EggTimeFunc>(
			_egg->deepcopy(), _fun, _arg);
	}

  private:
	void (*_fun)(double &, double &);
	double _arg;
};

////////////////////////////////////////////////

template <typename M, typename A>
class EggMemberModify : public IEggEventModify
{
  public:
	EggMemberModify(
		shared_ptr<IEgg> egg,
		M &(EventInfo::*getter)(),
		void (*fun)(M &, A &),
		A &arg)
		: IEggEventModify(egg),
		  //_evCopy(new Event()),
		  _getter(getter),
		  _fun(fun),
		  _arg(arg)
	{
	}

	shared_ptr<IEgg> deepcopy()
	{
		return make_shared<EggMemberModify<M, A>>(
			_egg->deepcopy(), _getter, _fun, _arg);
	}

	void modifyEvent(Event &ev)
	{
		auto infp = &ev._info;
		M &member = ((infp)->*_getter)();
		_fun(member, _arg);
	}

  private:
	M &(EventInfo::*_getter)();
	void (*_fun)(M &, A &);
	A _arg;
};

////////////////////////////////////////////////

class EggStart : public IEgg
{
  public:
	EggStart(shared_ptr<IEgg> part)
		: _part(part),
		  _gotStart(false) {}

	shared_ptr<IEgg> deepcopy()
	{
		return make_shared<EggStart>(_part->deepcopy());
	}

	void reset()
	{
		_part->reset();
		_gotStart = false;
	}

	Event pop()
	{
		if (_gotStart)
		{
			return _part->pop();
		}
		else
		{
			while (true)
			{
				auto ev = _part->pop();
				if (ev._info._isStart)
				{
					_gotStart = true;
					return ev;
				}
			}
		}
	}

  private:
	shared_ptr<IEgg> _part;
	bool _gotStart;
};

////////////////////////////////////////////////

#endif