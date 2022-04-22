#ifndef __WAVFILEWRITER_HPP__
#define __WAVFILEWRITER_HPP__

#include "Wave/IWaveSampler.hpp"
#include <string>
using namespace std;

void writeWavFile(IWaveSampler &sampler,
				   size_t samplesPerSecond,
				   size_t numSamples,
				   const string &fname);

#endif