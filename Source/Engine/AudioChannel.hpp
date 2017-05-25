/**
 * @file AudioChannel.hpp declares the audio channel class
 */

#pragma once

#include <QVector>

#include "WhistleLabel.hpp"


class AudioFile;

/**
 * @class AudioChannel is a single channel of an audio file
 */
class AudioChannel final
{
public:
  /**
   * @brief read deserializes the object
   * @param object the JSON object from which the object is deserialized
   * @param audioFile the audio file to which the channel belongs
   * @param channelNumber the number of the channel inside the file
   */
  void read(const QJsonObject& object, const AudioFile& audioFile, const unsigned int channelNumber);
  /**
   * @brief write serializes the object
   * @param object the JSON object to which the serialization is written
   */
  void write(QJsonObject& object) const;
  /// the index of the corresponding channel
  unsigned int channel = 0;
  /// the actual sequence of samples in the channel
  QVector<float> samples;
  /// the set of labeled whistles in the channel
  QVector<WhistleLabel> whistleLabels;
};
