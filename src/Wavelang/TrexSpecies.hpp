#ifndef __TREXSPECIES_HPP__
#define __TREXSPECIES_HPP__

#include "Common/SampleRate.hpp"
#include "ITrex.hpp"
#include "BezierPointSpec.hpp"
#include "Util/BezierCurves.hpp"
#include "RealFunc.hpp"
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

// Assume x goes rightward, sometimes going
// back to leftX suddenly, like a phase.
// X should never go beyond rightmost x in pts
class TrexBezierCurves0 : public ITrex
{
  public:
	TrexBezierCurves0(
		vector<BezierPointSpec0> pts,
		vector<bool> bs,
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

		if (_curPiece->_link)
		{
			// _result = bezierInterpol11(
			// 	*x1, *y1, 0, *x2, *y2, 0, x);
			_result = bezierInterpol11_flat(
				*x1, *y1, *x2, *y2, x);
		}
		else
		{
			_result = bezierInterpol0(
				*x1, *y1, *x2, *y2, x);
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
		bool _link;
	};

	vector<BezierPointSpec0> _pts;
	vector<bool> _bs;
	crealp _x;

	vector<BezierPiece0> _pieces;
	BezierPiece0 *_beginPiece;
	BezierPiece0 *_endPiece;
	BezierPiece0 *_curPiece;
	double _prevX;
};

////////////////////////////////////////////////

class TrexNoise : public ITrex
{
  public:
	TrexNoise() {
		srand(0);
	}

	~TrexNoise() {}

	void eval()
	{
		int n = 9000;
		int r = rand() % n;
		_result = (r*1.0)/n;
	}

	ITrex *copy()
	{
		return new TrexNoise();
	}

	TrexArgList getArgs()
	{
		return TrexArgList{};
	}

	bool introducesChange() const
	{
		return true;
	}

	void print(CrealpNamer &n) const
	{
		cout << "noise";
	}

};

////////////////////////////////////////////////

#endif