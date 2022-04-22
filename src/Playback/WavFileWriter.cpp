#include <fstream>
#include <iostream>
#include "WavFileWriter.hpp"
#include "WavHeader.hpp"
#include "Common/Crash.hpp"
using namespace std;

namespace little_endian_io
{
template <typename Word>
ostream &write_word(
	ostream &outs,
	Word value,
	unsigned size = sizeof(Word))
{
	for (; size; --size, value >>= 8)
		outs.put(static_cast<char>(value & 0xFF));
	return outs;
}
} // namespace little_endian_io
using namespace little_endian_io;

void writeWavFile(IWaveSampler &sampler,
				  size_t samplesPerSecond,
				  size_t numSamples,
				  const string &fname)
{
	ofstream f(fname, ios::binary);

	WavHeader wavHeader(samplesPerSecond, numSamples);
	f << wavHeader._bytes;

	for (size_t sampleI = 0; sampleI < numSamples; sampleI++)
	{
		sampler.genSample();
		if (sampler._left >= 1.0 || sampler._left < (-1.0))
		{
			CRASH("It is too FREAKING LOUD probably!!!")
		}
		if (sampler._right >= 1.0 || sampler._right < (-1.0))
		{
			CRASH("It is too FREAKING LOUD probably!!!")
		}

		write_word(f, (int)(sampler._left * 32768), 2);
		write_word(f, (int)(sampler._right * 32768), 2);

		if (sampleI % samplesPerSecond == 0)
		{
			cout << ".";
			cout.flush();
		}
	}
	f.close();
}
