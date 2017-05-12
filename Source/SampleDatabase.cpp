/**
 * @file SampleDatabase.cpp implements methods for the SampleDatabase
 */

#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include <sndfile.h>
#include <yaml-cpp/yaml.h>

#include "SampleDatabase.hpp"


SampleDatabase::SampleDatabase(const std::string& path)
  : dirpath(boost::filesystem::path(path).parent_path())
  , totalAudioLength(0)
  , totalWhistleLength(0)
{
  const YAML::Node root = YAML::LoadFile(path);
  if (!root.IsDefined() || !root.IsMap())
  {
    throw std::runtime_error("Root element of YAML file must be a map!");
  }
  const YAML::Node channels = root["audioChannels"];
  if (!channels.IsDefined() || !channels.IsSequence())
  {
    throw std::runtime_error("There must be a sequence called audioChannels in the file!");
  }
  for (const auto channel : channels)
  {
    if (!channel["path"].IsDefined() || !channel["channel"].IsDefined() || !channel["start"].IsDefined() || !channel["end"].IsDefined())
    {
      throw std::runtime_error("An element of the audioChannels sequence is not correct!");
    }
    const std::string filePath = channel["path"].as<std::string>();
    const unsigned int channelNumber = channel["channel"].as<unsigned int>();
    const unsigned int startSample = channel["start"].as<unsigned int>();
    const unsigned int endSample = channel["end"].as<unsigned int>();
    getAudioChannel(filePath, channelNumber).whistleLabels.emplace_back(startSample, endSample);
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

const std::list<AudioChannel>& SampleDatabase::getAudioChannels() const
{
  return audioChannels;
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
