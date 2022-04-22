#ifndef __WAVHEADER_HPP__
#define __WAVHEADER_HPP__

#include <string>
using namespace std;

// Constructs and contains the header for a WAV file.
// Does not include the samples, just the header.
class WavHeader
{
  public:
	WavHeader(size_t samplesPerSecond,
			  size_t numSamples);
	 // Size of whole file including data, in bytes
    size_t getFileSize() const;

  private:
	void writeString(const string &s);
	void writeWord(size_t value,
				   unsigned size);
	
	size_t _dataChunkSize;
	size_t _riffChunkSize;
	size_t _writePos;

  public:
	string _bytes;
};

#endif