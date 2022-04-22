#include "WavHeader.hpp"
using namespace std;

// See WAV format spec doc:
// http://soundfile.sapp.org/doc/WaveFormat/

WavHeader::WavHeader(size_t samplesPerSecond,
					 size_t numSamples)
	: _dataChunkSize(4 * numSamples), // NumSamples * NumChannels * BitsPerSample/8
	  _riffChunkSize(_dataChunkSize + 36),
	  _writePos(0),
	  _bytes(44, ' ') // blank string to start
{
	writeString("RIFF");
	writeWord(_riffChunkSize, 4);
	writeString("WAVEfmt ");
	writeWord(16, 4); // Subchunk1Size
	writeWord(1, 2); // PCM - integer samples
	writeWord(2, 2); // two channels (stereo file)
	writeWord(samplesPerSecond, 4); // SampleRate
	writeWord(samplesPerSecond * 4, 4); // SampleRate * NumChannels * BitsPerSample/8
	writeWord(4, 2); // BlockAlign == NumChannels * BitsPerSample/8
	writeWord(16, 2); // BitsPerSample (use a multiple of 8)

	// Data chunk header
	writeString("data");
	writeWord(_dataChunkSize, 4);
}

void WavHeader::writeString(const string &s)
{
	for (size_t i = 0; i < s.size(); i++)
	{
		_bytes[i + _writePos] = s[i];
	}
	_writePos += s.size();
}

void WavHeader::writeWord(
	size_t value,
	unsigned size)
{
	for (; size; --size, value >>= 8)
	{
		_bytes[_writePos++] = static_cast<char>(value & 0xFF);
	}
}

size_t WavHeader::getFileSize() const
{
	return _riffChunkSize + 8;
}