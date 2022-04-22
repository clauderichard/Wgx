#ifndef __VALUE_HPP__
#define __VALUE_HPP__

#include <memory>
#include "Common/All.hpp"
using namespace std;

////////////////////////////////////////////////

class ValueImpl
{
  public:
	virtual ~ValueImpl() {}
};

////////////////////////////////////////////////

template <typename T>
class ValueT : public ValueImpl
{
  public:
	ValueT(const T &v)
		: _value(v) {}
	~ValueT() {}

	T _value;
};

////////////////////////////////////////////////

class Value
{
  private:
	shared_ptr<ValueImpl> _valueImpl;

	Value(ValueImpl *vt)
		: _valueImpl(vt) {}

  public:
	Value()
		: _valueImpl(NULL) {}
	~Value()
	{
	}

	bool isNull() const
	{
		return _valueImpl == NULL;
	}

	template <typename T>
	static Value make(const T &v)
	{
		return Value(new ValueT<T>(v));
	}

	template <typename T>
	T &get()
	{
		ValueT<T> *v = dynamic_cast<ValueT<T> *>(_valueImpl.get());
		if (v == NULL)
		{
			CRASH("value.get() failed because the value is of an unexpected type")
		}
		return v->_value;
	}
};

////////////////////////////////////////////////

#endif