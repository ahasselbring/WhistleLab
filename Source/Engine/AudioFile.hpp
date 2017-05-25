/**
 * @file AudioFile.hpp declares the audio file class
 */

#pragma once

#include <QString>
#include <QList>
#include <QVector>

#include "AudioChannel.hpp"


class QDir;

/**
 * @class AudioFile represents a complete audio file
 */
class AudioFile final
{
public:
  /**
   * @brief read deserializes the object
   * @param object the JSON object from which the object is deserialized
   * @param basedir the directory relative to which paths are given
   */
  void read(const QJsonObject& object, const QDir& basedir);
  /**
   * @brief write serializes the object
   * @param object the JSON object to which the serialization is written
   */
  void write(QJsonObject& object) const;
  /// the path to the corresponding file
  QString path;
  /// the number of channels in the file
  unsigned int numberOfChannels = 0;
  /// the sample rate of the file
  unsigned int sampleRate = 0;
  /// the actual sequence of samples in the file
  QVector<float> samples;
  /// the channels of the file
  QList<AudioChannel> channels;
};
