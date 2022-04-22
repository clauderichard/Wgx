#ifndef __IEFFECT_HPP__
#define __IEFFECT_HPP__

#include "IWaveSampler.hpp"
#include "Util/Workforce.hpp"

class IEffect : public IWaveSampler
{
  public:
    IEffect()
		: _inputWaves(CAPACITY_INPUTS_PER_EFFECT)
	{}
	virtual ~IEffect() {}
	// virtual void genSample() = 0;

	virtual void initialize() = 0;

	void computeInputs()
	{
		_leftInput = 0;
		_rightInput = 0;
		FASTFOR_WORKERS_WORKING(_inputWaves, p)
		{
			_leftInput += (*p)->_left;
			_rightInput += (*p)->_right;
		}
	}

	Workforce<IWaveSampler *>::Worker addInputWave(IWaveSampler *inputWave)
	{
		auto grabbed = _inputWaves.grabWorker();
		grabbed.assign(inputWave);
		return grabbed;
	}

	const double *getLeftInputPtr()
	{
		return &_leftInput;
	}
	const double *getRightInputPtr()
	{
		return &_rightInput;
	}

  protected:
	double _leftInput;
	double _rightInput;
	Workforce<IWaveSampler *> _inputWaves;
};

#endif