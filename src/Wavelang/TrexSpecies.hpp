#ifndef __TREXSPECIES_HPP__
#define __TREXSPECIES_HPP__

#include "Common/SampleRate.hpp"
#include "Common/Capacities.hpp"
#include "Jurassic/ITrex.hpp"
#include "Util/BezierCurves.hpp"
#include "Jurassic/RealFunc.hpp"
#include <vector>
#include <cstdlib>
using namespace std;

////////////////////////////////////////////////

class TrexTimer : public ITrex
{
  public:
	TrexTimer()
		: _sampleCounter(0) {}
	~TrexTimer() {}

	void eval()
	{
		_sampleCounter++;
		_result = ((double)_sampleCounter) / WGX_SAMPLESPERSECOND;
	}

	ITrex *copy()
	{
		return new TrexTimer();
	}

	void reset()
	{
		_sampleCounter = 0;
	}

	TrexArgList getArgs()
	{
		return TrexArgList();
	}

	bool introducesChange() const
	{
		return true;
	}

	void print(CrealpNamer &n) const
	{
		cout << "timer";
	}

  private:
	size_t _sampleCounter;
};

////////////////////////////////////////////////

class TrexPhase : public ITrex
{
  public:
	TrexPhase(
		crealp freq)
		: _freq(freq) {}
	~TrexPhase() {}

	void eval()
	{
		_result += *_freq / WGX_SAMPLESPERSECOND;
		while (_result >= 1)
		{
			_result -= 1;
		}
	}

	ITrex *copy()
	{
		return new TrexPhase(_freq);
	}

	void reset()
	{
		_result = 0;
	}

	TrexArgList getArgs()
	{
		return TrexArgList{&_freq};
	}

	bool introducesChange() const
	{
		return true;
	}

	void print(CrealpNamer &n) const
	{
		cout << "phase " << n[_freq];
	}

  private:
	crealp _freq;
};

////////////////////////////////////////////////

class TrexPhaseT : public ITrex
{
  public:
	TrexPhaseT(
		crealp time)
		: _time(time),
		  _timeToSubtract(0) {}
	~TrexPhaseT() {}

	void eval()
	{
		_result = *_time - _timeToSubtract;
		while (_result >= 1)
		{
			_result -= 1;
			_timeToSubtract += 1;
		}
	}

	ITrex *copy()
	{
		return new TrexPhaseT(_time);
	}

	void reset()
	{
		_result = 0;
	}

	TrexArgList getArgs()
	{
		return TrexArgList{&_time};
	}

	bool introducesChange() const
	{
		return true;
	}

	void print(CrealpNamer &n) const
	{
		cout << "phaseT " << n[_time];
	}

  private:
	crealp _time;
	double _timeToSubtract;
};

////////////////////////////////////////////////

class TrexFunc1 : public ITrex
{
  public:
	TrexFunc1(
		double (*f)(double),
		crealp a)
		: _f(f), _a(a) {}

	~TrexFunc1() {}

	void eval()
	{
		_result = _f(*_a);
	}

	ITrex *copy()
	{
		return new TrexFunc1(_f, _a);
	}

	TrexArgList getArgs()
	{
		return TrexArgList{&_a};
	}

	bool introducesChange() const
	{
		return false;
	}

	void print(CrealpNamer &n) const
	{
		printRealFunc(_f);
		cout << n[_a];
	}

  private:
	double (*_f)(double);
	crealp _a;
};

////////////////////////////////////////////////

class TrexFunc2 : public ITrex
{
  public:
	TrexFunc2(
		double (*f)(double, double),
		crealp a,
		crealp b)
		: _f(f), _a(a), _b(b) {}

	~TrexFunc2() {}

	void eval()
	{
		_result = _f(*_a, *_b);
	}

	ITrex *copy()
	{
		return new TrexFunc2(_f, _a, _b);
	}

	TrexArgList getArgs()
	{
		return TrexArgList{&_a, &_b};
	}

	bool introducesChange() const
	{
		return false;
	}

	void print(CrealpNamer &n) const
	{
		cout << n[_a];
		printRealFunc(_f);
		cout << n[_b];
	}

  private:
	double (*_f)(double, double);
	crealp _a;
	crealp _b;
};

////////////////////////////////////////////////

struct BezierPointSpec0
{
	crealp _x;
	crealp _y;
};

enum BezierLinkType
{
	LINEAR = 0,
	QUADRATICFLATLEFT,
	QUADRATICFLATRIGHT,
	CUBICFLAT
};

// Assume x goes rightward, sometimes going
// back to leftX suddenly, like a phase.
// X should never go beyond rightmost x in pts
class TrexBezierCurves0 : public ITrex
{
  public:
	TrexBezierCurves0(
		vector<BezierPointSpec0> pts,
		vector<BezierLinkType> bs,
		crealp x)
		: _pts(pts),
		  _bs(bs),
		  _x(x),
		  _pieces(pts.size() - 1),
		  _prevX(*(pts[0]._x)) {}

	~TrexBezierCurves0() {}

	ITrex *copy()
	{
		return new TrexBezierCurves0(_pts, _bs, _x);
	}

	void initialize()
	{
		for (size_t i = 0; i < _pieces.size(); i++)
		{
			_pieces[i]._xLeft = _pts[i]._x;
			_pieces[i]._yLeft = _pts[i]._y;
			_pieces[i]._xRight = _pts[i + 1]._x;
			_pieces[i]._yRight = _pts[i + 1]._y;
			_pieces[i]._link = _bs[i];
		}
		_beginPiece = &_pieces[0];
		_endPiece = _beginPiece + _pieces.size();
		_curPiece = _beginPiece;
	}

	TrexArgList getArgs()
	{
		TrexArgList ret{&_x};
		for (size_t i = 0; i < _pts.size(); i++)
		{
			ret.push_back(&_pts[i]._x);
			ret.push_back(&_pts[i]._y);
		}
		return ret;
	}

	inline void findPiece(double x)
	{
		if (x < _prevX) // If x jumped to left
		{
			_curPiece = _beginPiece;
		}
		while (*_curPiece->_xRight < x)
		{
			_curPiece++;
			if (_curPiece == _endPiece)
			{
				throw out_of_range("Accessing Bezier curve beyond (right of) its range");
			}
		}

		if (*_x < *_beginPiece->_xLeft)
		{
			throw out_of_range("Accessing Bezier curve beyond (left of) its range");
		}
	}

	void eval()
	{
		double x = *_x;
		findPiece(x);

		auto x1 = _curPiece->_xLeft;
		auto y1 = _curPiece->_yLeft;
		auto x2 = _curPiece->_xRight;
		auto y2 = _curPiece->_yRight;

		switch(_curPiece->_link)
		{
			case LINEAR:
				_result = bezierInterpol_linear(
					*x1, *y1, *x2, *y2, x);
				break;
			case QUADRATICFLATLEFT:
				_result = bezierInterpol_quadraticLeftFlat(
					*x1, *y1, *x2, *y2, x);
				break;
			case QUADRATICFLATRIGHT:
				_result = bezierInterpol_quadraticLeftFlat(
					*x2, *y2, *x1, *y1, x);
				break;
			case CUBICFLAT:
				_result = bezierInterpol_cubicflat(
					*x1, *y1, *x2, *y2, x);
				break;
			default:
				CRASH("what you doing here?");
		}

		_prevX = x;
	}

	bool introducesChange() const
	{
		return false;
	}

	void print(CrealpNamer &n) const
	{
		cout << "bezier " << n[_x];
		for (auto p : _pts)
		{
			cout << " - " << n[p._x] << "," << n[p._y];
		}
	}

  private:
	struct BezierPiece0
	{
		crealp _xLeft;
		crealp _xRight;
		crealp _yLeft;
		crealp _yRight;
		BezierLinkType _link;
	};

	vector<BezierPointSpec0> _pts;
	vector<BezierLinkType> _bs;
	crealp _x;

	vector<BezierPiece0> _pieces;
	BezierPiece0 *_beginPiece;
	BezierPiece0 *_endPiece;
	BezierPiece0 *_curPiece;
	double _prevX;
};

////////////////////////////////////////////////

class TrexRandom : public ITrex
{
  public:
	TrexRandom(crealp minValue, crealp maxValue)
		: _minValue(minValue),
		  _maxValue(maxValue)
	{
		srand(0);
	}

	~TrexRandom() {}

	double getRand(double a, double b)
	{
		return a + (rand()*(b-a)) / RAND_MAX;
	}

	void eval()
	{
		_result = getRand(*_minValue, *_maxValue);
	}

	ITrex *copy()
	{
		return new TrexRandom(_minValue, _maxValue);
	}

	TrexArgList getArgs()
	{
		return TrexArgList{&_minValue, &_maxValue};
	}

	bool introducesChange() const
	{
		return true;
	}

	void print(CrealpNamer &n) const
	{
		cout << "random(" << n[_minValue] << "," << n[_maxValue] << ")";
	}
	
  private:
    crealp _minValue;
    crealp _maxValue;

};

////////////////////////////////////////////////

class TrexPrev : public ITrex
{
  public:
	TrexPrev(crealp arg)
		: _arg(arg),
		  _prevValue(0)
	{
	}

	~TrexPrev() {}

	void eval()
	{
		_result = _prevValue;
		_prevValue = *_arg;
	}

	ITrex *copy()
	{
		return new TrexPrev(_arg);
	}

	TrexArgList getArgs()
	{
		return TrexArgList{&_arg};
	}

	bool introducesChange() const
	{
		return true;
	}

	void print(CrealpNamer &n) const
	{
		cout << "prev " << n[_arg];
	}
	
  private:
    crealp _arg;
    double _prevValue;

};

////////////////////////////////////////////////

class TrexChangeWhenLeft : public ITrex
{
  public:
	TrexChangeWhenLeft(crealp cond, crealp val)
		: _cond(cond),
		  _val(val),
		  _evaled(false),
		  _prevCond(1)
	{
	}

	~TrexChangeWhenLeft() {}

	void eval()
	{
		if (!_evaled || _prevCond > *_cond)
		{
			_evaled = true;
			_result = *_val;
		}
		_prevCond = *_cond;
	}

	ITrex *copy()
	{
		return new TrexChangeWhenLeft(_cond, _val);
	}

	TrexArgList getArgs()
	{
		return TrexArgList{&_cond, &_val};
	}

	bool introducesChange() const
	{
		return true;
	}

	void print(CrealpNamer &n) const
	{
		cout << "onleft " << n[_cond] << " -> " << n[_val];
	}
	
  private:
    crealp _cond;
    crealp _val;
	bool _evaled;
	double _prevCond;

};

////////////////////////////////////////////////

class TrexDelay : public ITrex
{
  public:
	TrexDelay(crealp input, crealp delay, bool allocat = false)
		: _input(input),
		  _delay(delay),
		  _recordedSamples(allocat ? CAPACITY_TREXDELAY_BUFFERSIZE : 0, 0.0),
		  _writePos(0) {}
	~TrexDelay() {}

	void eval()
	{
		size_t delaySamples = (size_t)(*_delay * WGX_SAMPLESPERSECOND);
		if (delaySamples > CAPACITY_TREXDELAY_BUFFERSIZE)
		{
			cout << "CAPACITY_TREXDELAY_BUFFERSIZE ";
			cout << CAPACITY_TREXDELAY_BUFFERSIZE;
			cout << " cannot handle delay of ";
			cout << *_delay << " seconds." << endl;
			CRASH("Delay too big")
		}
		size_t readPos = (_writePos + CAPACITY_TREXDELAY_BUFFERSIZE - delaySamples) % CAPACITY_TREXDELAY_BUFFERSIZE;
		_result = _recordedSamples[readPos];
		_recordedSamples[_writePos] = *_input;
		_writePos = (_writePos + 1) % CAPACITY_TREXDELAY_BUFFERSIZE;
	}

	ITrex *copy()
	{
		return new TrexDelay(_input, _delay, true);
	}

	TrexArgList getArgs()
	{
		return TrexArgList{&_input, &_delay};
	}

	void reset()
	{
		_recordedSamples.assign(CAPACITY_TREXDELAY_BUFFERSIZE, 0.0);
		_writePos = 0;
	}

	bool introducesChange() const
	{
		return true;
	}

	void print(CrealpNamer &n) const
	{
		cout << "delay " << n[_delay] << " " << n[_input];
	}

  private:
	crealp _input;
	crealp _delay;
	vector<double> _recordedSamples;
	size_t _writePos;
};

////////////////////////////////////////////////

#endif