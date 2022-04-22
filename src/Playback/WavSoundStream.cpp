#ifdef SFML_SUPPORTED

#include "WavSoundStream.hpp"
#include "Common/All.hpp"
using namespace std;

////////////////////////////////////////////////

WavSoundStream::WavSoundStream(IWaveSampler &sampler,
							   size_t samplesPerSecond,
							   size_t numSamples)
	: _sampler(sampler),
	  _totalSamples(numSamples),
	  _samplesPerChunk(4096),
	  _chunkData(new Int16[_samplesPerChunk * 2]),
	  _chunkDataEnd(_chunkData + _samplesPerChunk * 2),
	  _pos(0)
{
	size_t numChannels = 2;
	initialize(numChannels, samplesPerSecond);
}

WavSoundStream::~WavSoundStream()
{
	delete[] _chunkData;
}

////////////////////////////////////////////////

bool WavSoundStream::onGetData(Chunk &data)
{
	cout.flush();
	auto siz = min(_samplesPerChunk, _totalSamples - _pos);
	bool mustKeepGoing = _samplesPerChunk < _totalSamples - _pos;
	Int16 *chunkPtr = _chunkData;
	while (chunkPtr != _chunkDataEnd)
	{
		_sampler.genSample();
		*(chunkPtr++) = makeSample(_sampler._left);
		*(chunkPtr++) = makeSample(_sampler._right);
	}

	data.samples = _chunkData;
	data.sampleCount = siz * 2;
	size_t secs1 = _pos / WGX_SAMPLESPERSECOND;
	_pos += siz;
	size_t secs2 = _pos / WGX_SAMPLESPERSECOND;
	if (secs1 < secs2)
	{
		cout << ".";
	}
	return mustKeepGoing;
}

void WavSoundStream::onSeek(Time timeOffset)
{
	cout << "seeked! what!" << endl;
}

////////////////////////////////////////////////

Int16 WavSoundStream::makeSample(double val)
{
	if (val < (-1) || val > 1)
	{
		logVal(val);
		CRASH("val was not between -1 and 1");
	}
	return (int)(val * 32768);
}

#endif