/**
 * @file SampleDatabase.cpp implements methods for the SampleDatabase
 */

#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <sndfile.h>

#include "SampleDatabase.hpp"


SampleDatabase::SampleDatabase(const std::string& path)
  : dirpath(boost::filesystem::path(path).parent_path())
  , totalAudioLength(0)
  , totalWhistleLength(0)
{
  std::ifstream file(path);
  if (!file.is_open())
  {
    throw std::runtime_error("Could not open sample database!");
  }

  std::string name;
  unsigned int channel, start, end;
  while (true)
  {
    file >> name >> channel >> start >> end;
    if (file.eof())
    {
      break;
    }
    getAudioChannel(name, channel).whistleLabels.emplace_back(start, end);
  }
  audioFiles.clear();

  for (auto& audioChannel : audioChannels)
  {
    totalAudioLength += audioChannel.samples.size();
    for (auto& whistleLabel : audioChannel.whistleLabels)
    {
      totalWhistleLength += whistleLabel.end - whistleLabel.start;
    }
  }
}

AudioSample SampleDatabase::getSample(const unsigned int length) const
{
  AudioSample as;
  as.label = 0;
  as.samples.resize(length, 0);
  as.sampleRate = 44100;
  return as;
}

AudioChannel& SampleDatabase::getAudioChannel(const std::string& name, const unsigned int channel)
{
  for (auto& audioChannel : audioChannels)
  {
    if (audioChannel.name == name && audioChannel.channel == channel)
    {
      return audioChannel;
    }
  }
  AudioFile& af = getAudioFile(name);
  if (channel >= af.numberOfChannels)
  {
    throw std::runtime_error("Channel is out of bounds!");
  }
  audioChannels.emplace_back();
  AudioChannel& ac = audioChannels.back();
  ac.name = name;
  ac.channel = channel;
  ac.sampleRate = af.sampleRate;
  ac.samples.resize(af.samples.size() / af.numberOfChannels);
  for (unsigned int i = 0; i < af.samples.size() / af.numberOfChannels; i++)
  {
    ac.samples[i] = af.samples[i * af.numberOfChannels + channel];
  }
  return ac;
}

AudioFile& SampleDatabase::getAudioFile(const std::string& name)
{
  for (auto& audioFile : audioFiles)
  {
    if (audioFile.name == name)
    {
      return audioFile;
    }
  }
  SF_INFO sfinfo;
  std::memset(&sfinfo, 0, sizeof(sfinfo));
  SNDFILE* f = sf_open((dirpath / name).c_str(), SFM_READ, &sfinfo);
  if (f == nullptr)
  {
    throw std::runtime_error("Could not open audio file!");
  }
  audioFiles.emplace_back();
  AudioFile& af = audioFiles.back();
  af.name = name;
  af.numberOfChannels = sfinfo.channels;
  af.sampleRate = sfinfo.samplerate;
  af.samples.resize(sfinfo.frames * sfinfo.channels);
  if (sf_readf_float(f, af.samples.data(), sfinfo.frames) != sfinfo.frames)
  {
    sf_close(f);
    throw std::runtime_error("Could not read samples from file!");
  }
  sf_close(f);
  return af;
}

float SampleDatabase::getLabel(const AudioChannel& audioChannel, const unsigned int start, const unsigned int end) const
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
