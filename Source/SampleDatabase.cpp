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
  const YAML::Node files = root["files"];
  if (!files.IsDefined() || !files.IsSequence())
  {
    throw std::runtime_error("There must be a sequence called files in the file!");
  }
  for (const auto file : files)
  {
    if (!file.IsDefined() || !file.IsMap()
      || !file["path"].IsDefined() || !file["path"].IsScalar()
      || !file["channels"].IsDefined() || !file["channels"].IsSequence())
    {
      throw std::runtime_error("An element of the files sequence is not correct!");
    }
    const std::string filePath = file["path"].as<std::string>();
    const YAML::Node channels = file["channels"];
    unsigned int channelNumber = 0;
    for (const auto channel : channels)
    {
      if (!channel.IsDefined() || !channel.IsMap()
        || !channel["whistles"].IsDefined() || !channel["whistles"].IsSequence())
      {
        throw std::runtime_error("An element of the channels sequence of a file is not correct!");
      }
      const YAML::Node whistles = channel["whistles"];
      for (const auto whistle : whistles)
      {
        if (!whistle.IsDefined() || !whistle.IsMap()
          || !whistle["start"].IsDefined() || !whistle["start"].IsScalar()
          || !whistle["end"].IsDefined() || !whistle["end"].IsScalar())
        {
          throw std::runtime_error("An element of the whistles sequence of a channel is not correct!");
        }
        const unsigned int startSample = whistle["start"].as<unsigned int>();
        const unsigned int endSample = whistle["end"].as<unsigned int>();
        getAudioChannel(filePath, channelNumber).whistleLabels.emplace_back(startSample, endSample);
      }
      channelNumber++;
    }
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
