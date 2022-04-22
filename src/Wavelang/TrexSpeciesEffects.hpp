#ifndef __TREXSPECIESEFFECTS_HPP__
#define __TREXSPECIESEFFECTS_HPP__

#include "Common/Capacities.hpp"
#include "Common/SampleRate.hpp"
#include "ITrex.hpp"
#include "BezierPointSpec.hpp"
#include "Util/BezierCurves.hpp"
#include "RealFunc.hpp"
#include <vector>
using namespace std;

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

class TrexEcho : public ITrex
{
  public:
	TrexEcho(crealp input, crealp gain, size_t delaySamples)
		: _input(input),
		  _gain(gain),
		  _delaySamples(delaySamples),
		  _recordedSamples(delaySamples + 1, 0.0),
		  _readPos(0),
		  _writePos(delaySamples) {}
	~TrexEcho() {}

	void eval()
	{
		_recordedSamples[_writePos] = *_input * *_gain;
		_result = *_input + _recordedSamples[_readPos];
		_readPos++;
		_writePos++;
		if (_readPos >= _recordedSamples.size())
			_readPos = 0;
		if (_writePos >= _recordedSamples.size())
			_writePos = 0;
	}

	ITrex *copy()
	{
		return new TrexEcho(_input, _gain, _delaySamples);
	}

	TrexArgList getArgs()
	{
		return TrexArgList{&_input, &_gain};
	}

	void reset()
	{
		_recordedSamples.assign(_delaySamples + 1, 0.0);
		_readPos = 0;
		_writePos = _delaySamples;
	}

	bool introducesChange() const
	{
		return true;
	}

	void print(CrealpNamer &n) const
	{
		cout << "echo";
	}

  private:
	crealp _input;
	crealp _gain;
	size_t _delaySamples;
	vector<double> _recordedSamples;
	size_t _readPos;
	size_t _writePos;
};

////////////////////////////////////////////////

class TrexEchoes : public ITrex
{
  public:
	TrexEchoes(crealp input, crealp gain, size_t delaySamples)
		: _input(input),
		  _gain(gain),
		  _delaySamples(delaySamples),
		  _recordedSamples(delaySamples + 1, 0.0),
		  _readPos(0),
		  _writePos(delaySamples) {}
	~TrexEchoes() {}

	void eval()
	{
		_result = *_input + (*_gain) * _recordedSamples[_readPos];
		_recordedSamples[_writePos] = _result;
		_readPos++;
		_writePos++;
		if (_readPos >= _recordedSamples.size())
			_readPos = 0;
		if (_writePos >= _recordedSamples.size())
			_writePos = 0;
	}

	ITrex *copy()
	{
		return new TrexEchoes(_input, _gain, _delaySamples);
	}

	void reset()
	{
		_recordedSamples.assign(_delaySamples + 1, 0.0);
		_readPos = 0;
		_writePos = _delaySamples;
	}

	TrexArgList getArgs()
	{
		return TrexArgList{&_input, &_gain};
	}

	bool introducesChange() const
	{
		return true;
	}

	void print(CrealpNamer &n) const
	{
		cout << "echoes";
	}

  private:
	crealp _input;
	crealp _gain;
	size_t _delaySamples;
	vector<double> _recordedSamples;
	size_t _readPos;
	size_t _writePos;
};

////////////////////////////////////////////////

#endif