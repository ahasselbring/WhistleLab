/**
 * @file SampleProvider.cpp implements methods for the SampleProvider
 */

#include <algorithm>

#include "SampleProvider.hpp"


SampleProvider::SampleProvider(const SampleDatabase& sampleDatabase)
  : sampleDatabase(sampleDatabase)
{
}

AudioSample SampleProvider::getSample(const unsigned int length) const
{
  AudioSample as;
  for (auto& audioChannel : sampleDatabase.getAudioChannels())
  {
    if (audioChannel.samples.size() < length)
    {
      continue;
    }
    as.label = getLabel(audioChannel, 0, length);
    as.samples.resize(length);
    std::copy(audioChannel.samples.begin(), audioChannel.samples.begin() + length, as.samples.begin());
    as.sampleRate = audioChannel.sampleRate;
    return as;
  }
  as.label = 0;
  as.samples.resize(length, 0);
  as.sampleRate = 44100;
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
