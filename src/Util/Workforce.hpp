#ifndef __WORKERITERATOR_HPP__
#define __WORKERITERATOR_HPP__

#include "Common/All.hpp"
#include <cstddef>
#include "FastArray.hpp"

// Array of interchangeable "workers" that you can turn individually on and off.
// Iterating through

////////////////////////////////////////////////

template <typename W>
class Workforce
{
  public:
	struct Worker
	{
		W &get()
		{
			return *_worker;
		}
		W &assign(W value)
		{
			*_worker = value;
			return *_worker;
		}
		void release()
		{
			_workforce->releaseWorkerByOfficialIndex(_officialIndex);
		}

	  private:
		Workforce<W> *_workforce;
		size_t _officialIndex;
		W *_worker;
		friend class Workforce<W>;
	};

	Workforce(size_t capacity)
		: _workersArray(capacity),
		  _officialToActual(capacity),
		  _actualToOfficial(capacity)
	{
		for (size_t i = 0; i < capacity; i++)
		{
			_officialToActual[i] = i;
			_actualToOfficial[i] = i;
		}
	}

	// Grabbing a worker returns you the next available one.
	// Returns the official index of the one returned.
	Worker grabWorker()
	{
		if (_workersArray._numSize == _workersArray._numCapacity)
		{
			cout << _workersArray._numCapacity << " was too small a workforce." << endl;
			CRASH("Ran out of workers!")
		}
		size_t newWorkerIndex = _workersArray._numSize;
		Worker ret;
		ret._workforce = this;
		ret._officialIndex = _actualToOfficial[newWorkerIndex];
		ret._worker = &_workersArray.pushBack();
		return ret;
	}

	// argument should be the index that grabWorker returned earlier.
	// Will stop that same worker, perhaps reordering the rest
	// (by putting the formerly-last worker in its place).
	void releaseWorkerByOfficialIndex(size_t officialWorkerIndex)
	{
		size_t actualWorkerIndex = _officialToActual[officialWorkerIndex];
		releaseWorkerByActualIndex(actualWorkerIndex);
	}

	void releaseWorkerByPointer(W *ptr)
	{
		size_t actualWorkerIndex = _workersArray.getIterIndex(ptr);
		releaseWorkerByActualIndex(actualWorkerIndex);
	}

	W &atActualIndex(size_t index)
	{
		return _workersArray._array[index];
	}

	FastArray<W> _workersArray;

	// one-to-one map between official worker index
	// (the one the user is returned from grabWorker)
	// and playing voice index (where it actually is in _voices)
	vector<size_t> _officialToActual;
	vector<size_t> _actualToOfficial;

  private:
	void releaseWorkerByActualIndex(size_t actualWorkerIndex)
	{
		_workersArray.popBack();
		if (actualWorkerIndex != _workersArray._numSize) // if voice was not the last
		{
			// swap actualWorkerIndex with _workersArray._numSize in official indexes
			size_t p1 = actualWorkerIndex;
			size_t p2 = _workersArray._numSize;
			size_t o1 = _actualToOfficial[actualWorkerIndex];
			size_t o2 = _actualToOfficial[_workersArray._numSize];
			_officialToActual[o1] = p2;
			_officialToActual[o2] = p1;
			_actualToOfficial[p1] = o2;
			_actualToOfficial[p2] = o1;
			_workersArray.swap(actualWorkerIndex, _workersArray._numSize);
		}
	}
};

////////////////////////////////////////////////

#define FASTFOR_WORKERS_WORKING(iter, pName) \
	FASTFOR_SIZE(iter._workersArray, pName)

#define FASTFOR_WORKERS_CAPACITY(iter, pName) \
	FASTFOR_CAPACITY(iter._workersArray, pName)

#define RELEASE_WORKER_INFASTFOR(iter, pName) \
	iter.releaseWorkerByPointer(pName);       \
	pName--;

////////////////////////////////////////////////

#endif