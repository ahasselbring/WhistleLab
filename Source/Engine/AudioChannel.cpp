/**
 * @file AudioChannel.cpp implements methods for the audio channel class
 */

#include <QJsonArray>
#include <QJsonObject>

#include "SampleDatabase.hpp"

#include "AudioChannel.hpp"


AudioChannel::AudioChannel(unsigned int channel)
  : channel(channel)
{
}

void AudioChannel::read(const QJsonObject& object, const AudioFile& audioFile)
{
  QJsonArray whistleLabelArray = object["whistles"].toArray();
  whistleLabels.resize(whistleLabelArray.size());
  for (int whistleLabelIndex = 0; whistleLabelIndex < whistleLabelArray.size(); whistleLabelIndex++)
  {
    QJsonObject whistleLabelObject = whistleLabelArray[whistleLabelIndex].toObject();
    WhistleLabel whistleLabel;
    whistleLabel.read(whistleLabelObject);
    whistleLabels[whistleLabelIndex] = whistleLabel;
  }
  // get the samples from the audio file that this channel belongs to
  samples.resize(audioFile.samples.size() / audioFile.numberOfChannels);
  for (unsigned int i = 0; i < samples.size(); i++)
  {
    samples[i] = audioFile.samples[i * audioFile.numberOfChannels + channel];
  }
}

void AudioChannel::write(QJsonObject& object) const
{
  QJsonArray whistleLabelArray;
  foreach (const WhistleLabel whistleLabel, whistleLabels)
  {
    QJsonObject whistleLabelObject;
    whistleLabel.write(whistleLabelObject);
    whistleLabelArray.append(whistleLabelObject);
  }
  object["whistles"] = whistleLabelArray;
}
