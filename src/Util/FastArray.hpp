#ifndef __FASTITERATINGARRAY_HPP__
#define __FASTITERATINGARRAY_HPP__

#include "Common/Crash.hpp"
#include <vector>
using namespace std;

////////////////////////////////////////////////

template <typename T>
struct FastArray
{
	FastArray(size_t capacity)
		: _numSize(0),
		  _numCapacity(capacity),
		  _array(capacity),
		  _begin(&_array[0]),
		  _endSize(&_array[0]),
		  _endCapacity(&_array[0] + capacity) {}

	FastArray<T> &operator=(FastArray<T> &other)
	{
		_numSize = other._numSize;
		_numCapacity = other._numCapacity;
		_array = other._array;
		_begin = &_array[0];
		_endSize = &_array[0] + _numSize;
		_endCapacity = &_array[0] + _numCapacity;
		return *this;
	}

	void resize(size_t newSize)
	{
		_numSize = newSize;
		_endSize = &_array[0] + _numSize;
	}

	T &pushBack() // increment size only
	{
		_numSize++;
		_endSize++;
		if (_numSize > _numCapacity)
		{
			cout << "Capacity of " << _numCapacity << endl;
			CRASH("FastArray capacity wasnt big enough")
		}
		return _array[_numSize - 1];
	}

	T &pushBack(T &t)
	{
		_array[_numSize++] = t;
		_endSize++;
		if (_numSize > _numCapacity)
		{
			cout << "Capacity of " << _numCapacity << endl;
			CRASH("FastArray capacity wasnt big enough")
		}
		return _array[_numSize - 1];
	}

	void popBack()
	{
		_numSize--;
		_endSize--;
	}

	void swap(size_t index1, size_t index2)
	{
		auto temp = _array[index1];
		_array[index1] = _array[index2];
		_array[index2] = temp;
	}

	size_t getIterIndex(T *ptr)
	{
		return ptr - _begin;
	}

	size_t _numSize;
	size_t _numCapacity;
	vector<T> _array;

	T *_begin;
	T *_endSize;
	T *_endCapacity;
};

////////////////////////////////////////////////

#define FASTFOR_SIZE(iter, pName) \
	for (auto pName = iter._begin; pName < iter._endSize; pName++)
// Loop 2 arrays (zip) assuming they're the same size
#define FASTFOR_SIZES_2(iter1, pName1, iter2, pName2) \
	auto pName2 = iter2._begin;                       \
	for (auto pName1 = iter1._begin;                  \
		 pName1 < iter1._endSize; pName1++, pName2++)

#define FASTFOR_CAPACITY(iter, pName) \
	for (auto pName = iter._begin; pName < iter._endCapacity; pName++)

////////////////////////////////////////////////

#endif