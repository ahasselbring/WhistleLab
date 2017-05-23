/**
 * @file AudioChannel.hpp declares the audio channel class
 */

#pragma once

#include <vector>

#include "WhistleLabel.hpp"


class AudioFile;

/**
 * @class AudioChannel is a single channel of an audio file
 */
class AudioChannel final
{
public:
  /**
   * @brief AudioChannel initializes members
   * @param channel the channel number
   */
  AudioChannel(unsigned int channel);
  /**
   * @brief read deserializes the object
   * @param object the JSON object from which the object is deserialized
   * @param audioFile the audio file to which the channel belongs
   */
  void read(const QJsonObject& object, const AudioFile& audioFile);
  /**
   * @brief write serializes the object
   * @param object the JSON object to which the serialization is written
   */
  void write(QJsonObject& object) const;
  /// the index of the corresponding channel
  const unsigned int channel;
  /// the actual sequence of samples in the channel
  std::vector<float> samples;
  /// the set of labeled whistles in the channel
  std::vector<WhistleLabel> whistleLabels;
};
