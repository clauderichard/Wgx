#ifndef __GRADIENTSPROCESSOR_HPP__
#define __GRADIENTSPROCESSOR_HPP__

#include "Common/Capacities.hpp"
#include "Common/SampleRate.hpp"
#include "Common/Crash.hpp"
#include "Util/BezierCurves.hpp"
#include "Jurassic/Realp.hpp"
#include "Util/Workforce.hpp"
#include "StaticBezierSpec.hpp"
#include <vector>
using namespace std;

////////////////////////////////////////////////

class IRealFunctionOfTime
{
  public:
    virtual ~IRealFunctionOfTime() {}

    virtual void eval(double time) = 0;

    double _result;
};

////////////////////////////////////////////////

struct StaticBezierPiece
{
    double _xLeft;
    double _xRight;
    double _yLeft;
    double _yRight;
    StaticBezierLinkType _link;
};

class BezierFunctionOfTime : public IRealFunctionOfTime
{
  public:
    BezierFunctionOfTime(
        const vector<StaticBezierPoint> pts,
		const vector<StaticBezierLinkType> bs)
	  : _pieces(pts.size() - 1),
        _curPiece(&_pieces[0]),
        _endPiece(_curPiece + _pieces.size()),
        _endTime(pts[pts.size()-1]._x)
    {
		for (size_t i = 0; i < _pieces.size(); i++)
		{
			_pieces[i]._xLeft = pts[i]._x;
			_pieces[i]._yLeft = pts[i]._y;
			_pieces[i]._xRight = pts[i + 1]._x;
			_pieces[i]._yRight = pts[i + 1]._y;
			_pieces[i]._link = bs[i];
		}
    }
    virtual ~BezierFunctionOfTime() {}

	void eval(double time)
	{
        if (time > _endTime)
        {
            throw false;
        }
		while (_curPiece->_xRight < time)
		{
			_curPiece++;
			if (_curPiece == _endPiece)
			{
				throw out_of_range("Accessing static Bezier curve beyond (right of) its range");
			}
		}

		auto x1 = _curPiece->_xLeft;
		auto y1 = _curPiece->_yLeft;
		auto x2 = _curPiece->_xRight;
		auto y2 = _curPiece->_yRight;

		switch(_curPiece->_link)
		{
			case S_LINEAR:
				_result = bezierInterpol_linear(
					x1, y1, x2, y2, time);
				break;
			case S_QUADRATICFLATLEFT:
				_result = bezierInterpol_quadraticLeftFlat(
					x1, y1, x2, y2, time);
				break;
			case S_QUADRATICFLATRIGHT:
				_result = bezierInterpol_quadraticLeftFlat(
					x2, y2, x1, y1, time);
				break;
			case S_CUBICFLAT:
				_result = bezierInterpol_cubicflat(
					x1, y1, x2, y2, time);
				break;
			default:
				CRASH("what you doing here?");
		}
	}

    vector<StaticBezierPiece> _pieces;
	StaticBezierPiece *_curPiece;
	StaticBezierPiece *_endPiece;
    double _endTime;
};

////////////////////////////////////////////////

class GradientProcess
{
  public:
    GradientProcess() {}
    ~GradientProcess() {}

    void init(realp varPtr,
        shared_ptr<IRealFunctionOfTime> func,
        double timeSpeedupFactor)
    {
		_curSampleI = 0;
        _varPtr = varPtr;
        _func = func;
        _sampleToTimeFactor = timeSpeedupFactor/WGX_SAMPLESPERSECOND;

    }

    void processNext()
    {
        _curSampleI++;
        _func->eval(_curSampleI*_sampleToTimeFactor);
        *_varPtr = _func->_result;
    }

  private:
    realp _varPtr;
    shared_ptr<IRealFunctionOfTime> _func;
    double _sampleToTimeFactor;
    size_t _curSampleI;
};

#endif