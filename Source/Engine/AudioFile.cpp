/**
 * @file AudioFile.cpp implements methods for the audio file class
 */

#include <cstring>

#include <sndfile.h>

#include <QDir>
#include <QJsonArray>
#include <QJsonObject>

#include "AudioFile.hpp"


void AudioFile::read(const QJsonObject& object, const QDir& basedir)
{
  path = object["path"].toString();
  QJsonArray channelArray = object["channels"].toArray();
  SF_INFO sfinfo;
  std::memset(&sfinfo, 0, sizeof(sfinfo));
  SNDFILE* f = sf_open(basedir.filePath(path).toStdString().c_str(), SFM_READ, &sfinfo);
  if (f == nullptr)
  {
    throw std::runtime_error("Could not open audio file!");
  }
  numberOfChannels = sfinfo.channels;
  if (numberOfChannels != static_cast<unsigned int>(channelArray.size()))
  {
    throw std::runtime_error("Audio file has different number of channels than indicated in sample database!");
  }
  sampleRate = sfinfo.samplerate;
  samples.resize(static_cast<int>(sfinfo.frames * sfinfo.channels));
  if (sf_readf_float(f, samples.data(), sfinfo.frames) != sfinfo.frames)
  {
    sf_close(f);
    throw std::runtime_error("Could not read samples from file!");
  }
  sf_close(f);
  channels.clear();
  for (int channelIndex = 0; channelIndex < channelArray.size(); channelIndex++)
  {
    QJsonObject audioChannelObject = channelArray[channelIndex].toObject();
    AudioChannel audioChannel;
    audioChannel.read(audioChannelObject, *this, static_cast<unsigned int>(channelIndex));
    channels.append(audioChannel);
  }
}

void AudioFile::write(QJsonObject& object) const
{
  object["path"] = path;
  QJsonArray channelArray;
  foreach (const AudioChannel audioChannel, channels)
  {
    QJsonObject audioChannelObject;
    audioChannel.write(audioChannelObject);
    channelArray.append(audioChannelObject);
  }
  object["channels"] = channelArray;
}
