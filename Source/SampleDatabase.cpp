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
    SF_INFO sfinfo;
    std::memset(&sfinfo, 0, sizeof(sfinfo));
    SNDFILE* f = sf_open((dirpath / filePath).c_str(), SFM_READ, &sfinfo);
    if (f == nullptr)
    {
      throw std::runtime_error("Could not open audio file!");
    }
    audioFiles.emplace_back();
    AudioFile& af = audioFiles.back();
    af.name = filePath;
    af.numberOfChannels = sfinfo.channels;
    if (af.numberOfChannels != channels.size())
    {
      throw std::runtime_error("Audio file has different number of channels than indicated in sample database!");
    }
    af.sampleRate = sfinfo.samplerate;
    af.samples.resize(sfinfo.frames * sfinfo.channels);
    if (sf_readf_float(f, af.samples.data(), sfinfo.frames) != sfinfo.frames)
    {
      sf_close(f);
      throw std::runtime_error("Could not read samples from file!");
    }
    sf_close(f);
    unsigned int channelNumber = 0;
    for (const auto channel : channels)
    {
      if (!channel.IsDefined() || !channel.IsMap()
        || !channel["whistles"].IsDefined() || !channel["whistles"].IsSequence())
      {
        throw std::runtime_error("An element of the channels sequence of a file is not correct!");
      }
      af.channels.emplace_back();
      AudioChannel& ac = af.channels.back();
      ac.channel = channelNumber;
      ac.samples.resize(af.samples.size() / af.numberOfChannels);
      for (unsigned int i = 0; i < af.samples.size() / af.numberOfChannels; i++)
      {
        ac.samples[i] = af.samples[i * af.numberOfChannels + channelNumber];
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
        const int startSample = whistle["start"].as<int>();
        const int endSample = whistle["end"].as<int>();
        ac.whistleLabels.emplace_back(startSample, endSample);
      }
      channelNumber++;
    }
  }

  for (auto& audioFile : audioFiles)
  {
    for (auto& audioChannel : audioFile.channels)
    {
      totalAudioLength += audioChannel.samples.size();
      for (auto& whistleLabel : audioChannel.whistleLabels)
      {
        totalWhistleLength += whistleLabel.end - whistleLabel.start;
      }
    }
  }
}

const std::list<AudioFile>& SampleDatabase::getAudioFiles() const
{
  return audioFiles;
}
