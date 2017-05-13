/**
 * @file SampleProvider.cpp implements methods for the SampleProvider
 */

#include <algorithm>

#include "SampleProvider.hpp"


SampleProvider::SampleProvider(const SampleDatabase& sampleDatabase)
  : sampleDatabase(sampleDatabase)
{
}

AudioSample SampleProvider::getSampleByOffset(const unsigned int offset, const unsigned int length) const
{
  AudioSample as;
  unsigned int remainingOffset = offset;
  for (auto& audioChannel : sampleDatabase.getAudioChannels())
  {
    if (remainingOffset + length <= audioChannel.samples.size())
    {
      as.label = getLabel(audioChannel, remainingOffset, remainingOffset + length);
      as.samples.resize(length);
      std::copy(audioChannel.samples.begin() + remainingOffset, audioChannel.samples.begin() + remainingOffset + length, as.samples.begin());
      as.sampleRate = audioChannel.sampleRate;
      return as;
    }
    else
    {
      if (remainingOffset < audioChannel.samples.size())
      {
        remainingOffset = 0;
      }
      else
      {
        remainingOffset -= audioChannel.samples.size();
      }
    }
  }
  as.sampleRate = 0;
  return as;
}

float SampleProvider::getLabel(const AudioChannel& audioChannel, const unsigned int start, const unsigned int end) const
{
  const unsigned int totalLength = end - start;
  unsigned int whistleSamples = 0;
  for (auto& whistleLabel : audioChannel.whistleLabels)
  {
    if (start < whistleLabel.end && whistleLabel.start < end)
    {
      whistleSamples += std::min(whistleLabel.end, end) - std::max(start, whistleLabel.start);
    }
  }
  return static_cast<float>(whistleSamples) / totalLength;
}
