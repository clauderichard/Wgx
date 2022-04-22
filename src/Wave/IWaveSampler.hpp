#ifndef __IWAVESAMPLER_HPP__
#define __IWAVESAMPLER_HPP__

////////////////////////////////////////////////

class IWaveSampler
{
  public:
	// Current sample value for left and right ears
	double _left;
	double _right;

	/* Should set _left and _right do next sample value.
	 * Each value should be between -1.0 and 1.0
	 * (1.0 and -1.0 correspond to max amplitude in any wav file)
	 * Basic benchmark shows modifying this guy's members
	 * then reading them is the fastest-running.
	 * You should make this method super efficient,
	 * because you will probably call this 44,100 times per second.
	 */
	virtual void genSample() = 0;
};

////////////////////////////////////////////////

#endif