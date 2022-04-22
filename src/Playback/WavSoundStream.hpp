#ifdef SFML_SUPPORTED

#ifndef __WAVSOUNDSTREAM_HPP__
#define __WAVSOUNDSTREAM_HPP__

#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include "Wave/IWaveSampler.hpp"
#include "WavHeader.hpp"
using namespace std;
using namespace sf;

class WavSoundStream : public sf::SoundStream
{
	
  public:
	WavSoundStream(IWaveSampler &sampler,
				   size_t samplesPerSecond,
				   size_t numSamples);
	~WavSoundStream();

  protected:
	bool onGetData(Chunk &data);
	void onSeek(Time timeOffset);

  private:
	Int16 makeSample(double val);
	
	IWaveSampler &_sampler;
	size_t _totalSamples;
	size_t _samplesPerChunk;
	Int16 *_chunkData;
	Int16* _chunkDataEnd;
	size_t _pos;
};

#endif

#endif