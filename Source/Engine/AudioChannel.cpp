/**
 * @file AudioChannel.cpp implements methods for the audio channel class
 */

#include <QJsonArray>
#include <QJsonObject>

#include "AudioFile.hpp"

#include "AudioChannel.hpp"


void AudioChannel::read(const QJsonObject& object, const AudioFile& audioFile, const unsigned int channelNumber)
{
  channel = channelNumber;
  // get the samples from the audio file that this channel belongs to
  samples.resize(audioFile.samples.size() / audioFile.numberOfChannels);
  for (int i = 0; i < samples.size(); i++)
  {
    samples[i] = audioFile.samples[i * audioFile.numberOfChannels + channel];
  }
  QJsonArray whistleLabelArray = object["whistleLabels"].toArray();
  whistleLabels.resize(whistleLabelArray.size());
  for (int whistleLabelIndex = 0; whistleLabelIndex < whistleLabelArray.size(); whistleLabelIndex++)
  {
    QJsonObject whistleLabelObject = whistleLabelArray[whistleLabelIndex].toObject();
    WhistleLabel whistleLabel;
    whistleLabel.read(whistleLabelObject);
    whistleLabels[whistleLabelIndex] = whistleLabel;
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
  object["whistleLabels"] = whistleLabelArray;
}
